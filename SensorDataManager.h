#include <deque>
#include <mutex>
#include <algorithm>
#include <vector>
#include <queue>
#include <cmath>

class SensorDataManager {
public:
    void MeasureDensityReady(int density, int time_uS);
    void MeasurePositionReady(int position_mm, int time_uS);
    void CalculateDensityValues(int min_pos_mm, int max_pos_mm,
                                int* mean_density, int* min_density, int* median_density);

private:
    std::deque<std::pair<int, int>> density_buffer;   // {time_uS, density}
    std::deque<std::pair<int, int>> position_buffer;  // {time_uS, position_mm}
    std::mutex data_mutex;

    static constexpr int WINDOW_US = 5'000'000;  // 5 seconds in microseconds

    void trim_old_data(int now_us);
    int interpolate_position(int time_us);
};