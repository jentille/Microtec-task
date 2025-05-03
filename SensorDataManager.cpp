#include "SensorDataManager.h"
#include <climits>

void SensorDataManager::MeasureDensityReady(int density, int time_uS) {
    std::lock_guard<std::mutex> lock(data_mutex);
    trim_old_data(time_uS);
    density_buffer.emplace_back(time_uS, density);
}

void SensorDataManager::MeasurePositionReady(int position_mm, int time_uS) {
    std::lock_guard<std::mutex> lock(data_mutex);
    trim_old_data(time_uS);
    position_buffer.emplace_back(time_uS, position_mm);
}

void SensorDataManager::trim_old_data(int now_us) {
    while (!density_buffer.empty() && density_buffer.front().first < now_us - WINDOW_US) {
        density_buffer.pop_front();
    }
    while (!position_buffer.empty() && position_buffer.front().first < now_us - WINDOW_US) {
        position_buffer.pop_front();
    }
}

int SensorDataManager::interpolate_position(int time_us) {
    if (position_buffer.empty()) return -1;
    if (time_us <= position_buffer.front().first) return position_buffer.front().second;
    if (time_us >= position_buffer.back().first) return position_buffer.back().second;

    auto it = std::lower_bound(position_buffer.begin(), position_buffer.end(), time_us,
        [](const std::pair<int, int>& a, int t) { return a.first < t; });

    auto after = *it;
    auto before = *(it - 1);

    double ratio = (double)(time_us - before.first) / (after.first - before.first);
    return static_cast<int>(before.second + ratio * (after.second - before.second));
}

void SensorDataManager::CalculateDensityValues(int min_pos_mm, int max_pos_mm,
                                               int* mean_density, int* min_density, int* median_density) {
    std::lock_guard<std::mutex> lock(data_mutex);

    std::vector<int> relevant_densities;
    int sum = 0, count = 0, min_val = INT_MAX;

    for (const auto& [t, d] : density_buffer) {
        int pos = interpolate_position(t);
        if (pos >= min_pos_mm && pos <= max_pos_mm) {
            relevant_densities.push_back(d);
            sum += d;
            min_val = std::min(min_val, d);
            ++count;
        }
    }

    if (count == 0) {
        *mean_density = *min_density = *median_density = 0;
        return;
    }

    std::nth_element(relevant_densities.begin(), relevant_densities.begin() + count / 2, relevant_densities.end());
    int median = relevant_densities[count / 2];

    *mean_density = sum / count;
    *min_density = min_val;
    *median_density = median;
}