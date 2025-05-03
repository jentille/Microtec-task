/**
    SensorDataManager.h

    Interface for a thread-safe data manager class used to collect, align, and analyze
    asynchronous sensor data streams (density and position) in a longitudinal board scanning system.

    Features:
    - Buffering of recent data in timestamp order using std::deque
    - Thread safety through internal mutex protection
    - Sliding window filtering and linear interpolation
    - Statistical summary (mean, min, median) for density values in a position range
*/

#include <deque>
#include <mutex>
#include <algorithm>
#include <vector>
#include <queue>
#include <cmath>
#include <climits>

class SensorDataManager {
    public:
        /**
            Registers a new density measurement.
            @param density - Sensor reading (integer scale)
            @param time_uS - Timestamp in microseconds
        */
        void MeasureDensityReady(int density, int time_uS);

        /**
            Registers a new board position measurement.
            @param position_mm - Board position in millimeters
            @param time_uS - Timestamp in microseconds
        */
        void MeasurePositionReady(int position_mm, int time_uS);

        /**
            Computes mean, min, and median of density values whose interpolated positions
            fall within the range [min_pos_mm, max_pos_mm].
            
            Thread-safe; allows dynamic median strategies through MedianStrategy.h.
        */
        void CalculateDensityValues(int min_pos_mm, int max_pos_mm, int* mean_density, int* min_density, int* median_density);

    private:
        // Buffers storing recent (timestamp, value) pairs
        std::deque<std::pair<int, int>> density_buffer;   // {time_uS, density}
        std::deque<std::pair<int, int>> position_buffer;  // {time_uS, position_mm}

        // Mutex guarding access to both buffers
        std::mutex data_mutex;

    // Sliding window length for keeping recent data (default: 5 seconds)
    static constexpr int WINDOW_US = 5'000'000;  // 5 seconds in microseconds

    // Sliding window length for keeping recent data (default: 5 seconds)
    void trim_old_data(int now_us);

    /**
        Linearly interpolates the board position at a given timestamp
        using nearest neighbor timestamps in the position buffer.

        @param time_us - Timestamp in microseconds
        @return Interpolated position in millimeters
    */
    int interpolate_position(int time_us);
};