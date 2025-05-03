/**
    SensorDataManager.cpp
 
    A thread-safe class implementation that manages real-time data buffers for density and position
    measurements in a longitudinal wood board scanning system. This class stores and aligns
    asynchronous data streams using timestamps and performs statistical calculations over the an
    arbitrary window of time configurable in SensorDataManager.h (default 5-seconds).

    Features:
    - Efficient memory usage with timestamp-trimmed buffers (std::deque)
    - Thread-safe insertion of density and position measurements via mutex locking
    - Interpolation of positions for non-aligned timestamps
    - Calculation of mean, min, and median densities in a specified position interval
*/

#include "SensorDataManager.h"
#include "MedianStrategy.h"

void SensorDataManager::MeasureDensityReady(int density, int time_uS) {
    /**
        Callback for density data arrival.
        Locks access to buffers, removes stale data, and appends the new density reading.
    */
    std::lock_guard<std::mutex> lock(data_mutex);
    trim_old_data(time_uS);
    density_buffer.emplace_back(time_uS, density);
}

void SensorDataManager::MeasurePositionReady(int position_mm, int time_uS) {
    /**
        Callback for position data arrival.
        Locks access to buffers, removes stale data, and appends the new position reading.
    */
    std::lock_guard<std::mutex> lock(data_mutex);
    trim_old_data(time_uS);
    position_buffer.emplace_back(time_uS, position_mm);
}

void SensorDataManager::trim_old_data(int now_us) {
    /**
        Removes any measurements older than the sliding 5-second window from both buffers.
    */
    while (!density_buffer.empty() && density_buffer.front().first < now_us - WINDOW_US) {
        density_buffer.pop_front();
    }
    while (!position_buffer.empty() && position_buffer.front().first < now_us - WINDOW_US) {
        position_buffer.pop_front();
    }
}

int SensorDataManager::interpolate_position(int time_us) {
    /**
        Interpolates the board position at a given timestamp using the two nearest position samples.
        Falls back to bounds if requested time is outside the known range.

        @param time_us - The timestamp in microseconds to interpolate for.
        @return Estimated position in mm at the given timestamp.
    */

    // Compare function used in std::lower_bound:
    // Returns true if the entry timestamp is still less than the one we’re searching for.
    auto compareTime = [](const std::pair<int, int>& entry, int timestamp) {
        return entry.first < timestamp;
    };

    // Return early if there’s no data
    if (position_buffer.empty()) return -1;

    // Clamp if the time is outside our known position range
    if (time_us <= position_buffer.front().first) 
        return position_buffer.front().second;

    if (time_us >= position_buffer.back().first) 
        return position_buffer.back().second;

    // Find the pointer to the timestamp which is >= time_us in the position buffer
    auto after = std::lower_bound(position_buffer.begin(), position_buffer.end(), time_us, compareTime);
    // The pointer just before is the timestamp which is < time_us in the position buffer
    auto before = after - 1;

    // Computes how far between the two known position timestamps before and after our target time_us falls.
    double ratio = (double)(time_us - before->first) / (after->first - before->first);
    // Linearly interpolate the position of the board at time_us using the ratio
    return static_cast<int>(before->second + ratio * (after->second - before->second));
}

void SensorDataManager::CalculateDensityValues(int min_pos_mm, int max_pos_mm, int* mean_density, int* min_density, int* median_density) {
    /**
        Asynchronous query function that computes the mean, min, and median density values for all
        density samples whose interpolated position falls within [min_pos_mm, max_pos_mm].
        
        Thread-safe and optimized for median computation using nth_element (partial sort).
        
        @param min_pos_mm - Lower bound of board position range.
        @param max_pos_mm - Upper bound of board position range.
        @param mean_density - Output pointer for the average density.
        @param min_density - Output pointer for the minimum density.
        @param median_density - Output pointer for the median density.
    */

    // Lock access to shared data
    std::lock_guard<std::mutex> lock(data_mutex);

    // relevant_densities: Stores all the densities that fall within the requested board section
    // sum, count: Used to compute the mean
    // min_val: Used to track the minimum density value
    std::vector<int> relevant_densities;
    relevant_densities.reserve(density_buffer.size());
    int sum = 0, count = 0, min_val = INT_MAX;

    // Iterate through all [timestamp, density] pairs in the density buffer
    for (const auto& [timestamp, current_density] : density_buffer) {
        // Interpolate the position in mm of this density reading
        int pos = interpolate_position(timestamp);
        // Only include this sample if the estimated position is inside the [min_pos_mm, max_pos_mm] interval
        if (pos >= min_pos_mm && pos <= max_pos_mm) {
            // Add to working set
            relevant_densities.push_back(current_density);
            sum += current_density;
            min_val = std::min(min_val, current_density);
            ++count;
        }
    }

    if (count == 0) {
        // If no densities matched the position interval, just set all stats to zero
        *mean_density = *min_density = *median_density = 0;
        return;
    }

    // Compute the median density using the selected strategy (NthElement by default),
    // which partially sorts the vector in average O(n) for efficient median extraction
    int median = MedianStrategy::compute(relevant_densities, MedianAlgorithm::NthElement);

    *mean_density = sum / count;
    *min_density = min_val;
    *median_density = median;
}