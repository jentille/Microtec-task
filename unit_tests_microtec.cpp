#include <thread>
#include <chrono>
#include "SensorDataManager.h"

SensorDataManager manager;

void simulate_density_input() {
    for (int i = 0; i < 1000; ++i) {
        manager.MeasureDensityReady(rand() % 200, i * 1000);
        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
}

void simulate_position_input() {
    for (int i = 0; i < 300; ++i) {
        manager.MeasurePositionReady(i, i * 3000);
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
}

void simulate_query() {
    for (int i = 0; i < 100; ++i) {
        int mean, min, median;
        manager.CalculateDensityValues(10, 200, &mean, &min, &median);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    std::thread t1(simulate_density_input);
    std::thread t2(simulate_position_input);
    std::thread t3(simulate_query);

    t1.join();
    t2.join();
    t3.join();
}