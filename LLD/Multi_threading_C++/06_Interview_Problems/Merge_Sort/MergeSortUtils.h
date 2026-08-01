#ifndef MULTI_THREADING_MERGE_SORT_UTILS_H
#define MULTI_THREADING_MERGE_SORT_UTILS_H

#include <chrono>
#include <iostream>
#include <random>
#include <vector>

namespace merge_sort_demo {

inline void merge(std::vector<int>& arr, int left, int mid, int right) {
    std::vector<int> tmp(static_cast<size_t>(right - left + 1));
    int i = left;
    int j = mid + 1;
    int k = 0;

    while (i <= mid && j <= right) {
        if (arr[static_cast<size_t>(i)] <= arr[static_cast<size_t>(j)]) {
            tmp[static_cast<size_t>(k++)] = arr[static_cast<size_t>(i++)];
        } else {
            tmp[static_cast<size_t>(k++)] = arr[static_cast<size_t>(j++)];
        }
    }
    while (i <= mid) {
        tmp[static_cast<size_t>(k++)] = arr[static_cast<size_t>(i++)];
    }
    while (j <= right) {
        tmp[static_cast<size_t>(k++)] = arr[static_cast<size_t>(j++)];
    }
    for (int t = 0; t < k; ++t) {
        arr[static_cast<size_t>(left + t)] = tmp[static_cast<size_t>(t)];
    }
}

inline void sequentialMergeSort(std::vector<int>& arr, int left, int right) {
    if (left >= right) {
        return;
    }
    const int mid = left + (right - left) / 2;
    sequentialMergeSort(arr, left, mid);
    sequentialMergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

inline std::vector<int> makeRandomArray(int n, unsigned seed = 42) {
    std::vector<int> arr(static_cast<size_t>(n));
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, 1'000'000);
    for (int& v : arr) {
        v = dist(rng);
    }
    return arr;
}

inline bool isSorted(const std::vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i] < arr[i - 1]) {
            return false;
        }
    }
    return true;
}

inline void printHead(const std::vector<int>& arr, int count = 12) {
    const int n = static_cast<int>(std::min(arr.size(), static_cast<size_t>(count)));
    for (int i = 0; i < n; ++i) {
        std::cout << arr[static_cast<size_t>(i)] << " ";
    }
    if (static_cast<int>(arr.size()) > n) {
        std::cout << "...";
    }
    std::cout << "\n";
}

template <typename Fn>
inline long long timeMs(Fn&& fn) {
    const auto start = std::chrono::steady_clock::now();
    fn();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now() - start)
        .count();
}

}  // namespace merge_sort_demo

#endif  // MULTI_THREADING_MERGE_SORT_UTILS_H
