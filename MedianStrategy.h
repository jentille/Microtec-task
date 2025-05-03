/**
    MedianStrategy.h

    This file defines a flexible strategy class for computing the median from a dataset.
    It supports multiple algorithms to compute the median efficiently depending on use-case
    constraints (speed, stability, or memory usage).

    Features:
    - NthElement: Average-case O(n) median using std::nth_element
    - FullSort: Simpler but slower std::sort-based median
    - HeapMedian: Online-style median using two heaps (max/min)

    The strategy is intended for integration with real-time sensor data processing pipelines
    like those found in SensorDataManager.cpp.
*/

#ifndef MEDIAN_STRATEGY_H
#define MEDIAN_STRATEGY_H

#include <vector>
#include <queue>
#include <algorithm>

/**
    Enum representing the supported median algorithms.
    - NthElement: Uses partial sort (efficient for large datasets)
    - FullSort: Fully sorts the data before extracting median
    - HeapMedian: Uses two heaps for robust, real-time median tracking
*/
enum class MedianAlgorithm {
    NthElement,
    FullSort,
    HeapMedian
};


/**
    MedianStrategy provides a static method `compute` for calculating the median of a vector
    using a specified algorithm from the MedianAlgorithm enum.
*/
class MedianStrategy {
public:

    /**
        Computes the median value of the given data vector using the selected algorithm.

        @param data - Reference to the data vector (can be reordered in-place)
        @param algorithm - Algorithm type from the MedianAlgorithm enum
        @return Integer median value (lower middle in case of even-length vector)
    */
    static int compute(std::vector<int>& data, MedianAlgorithm algorithm) {
        int n = data.size();
        if (n == 0) return 0;

        switch (algorithm) {
            case MedianAlgorithm::NthElement:
                // Partially sorts the vector so the nth element is in correct position
                std::nth_element(data.begin(), data.begin() + n / 2, data.end());
                return data[n / 2];

            case MedianAlgorithm::FullSort:
                // Fully sorts the vector and returns the middle element
                std::sort(data.begin(), data.end());
                return data[n / 2];

            case MedianAlgorithm::HeapMedian: {
                // Maintains two heaps:
                // - max-heap for lower half
                // - min-heap for upper half
                std::priority_queue<int> lower;
                std::priority_queue<int, std::vector<int>, std::greater<int>> upper;

                for (int val : data) {
                    if (lower.empty() || val < lower.top())
                        lower.push(val);
                    else
                        upper.push(val);
                    
                    // Balance the heaps so their sizes differ by at most one
                    if (lower.size() > upper.size() + 1) {
                        upper.push(lower.top()); lower.pop();
                    } else if (upper.size() > lower.size()) {
                        lower.push(upper.top()); upper.pop();
                    }
                }
                
                // Return top of max-heap as the median
                return lower.top();
            }

            default:
                return 0;
        }
    }
};

#endif // MEDIAN_STRATEGY_H