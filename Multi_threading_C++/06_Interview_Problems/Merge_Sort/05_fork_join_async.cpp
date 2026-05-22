/**
 * 05 — Fork-join style using std::async (C++17 task-based parallelism).
 */
#include <future>
#include <iostream>

#include "MergeSortUtils.h"

using namespace std;
using namespace merge_sort_demo;

namespace {

constexpr int kThreshold = 256;

void parallelMergeSortAsync(vector<int>& arr, int left, int right, int threshold) {
    if (left >= right) {
        return;
    }
    if (right - left + 1 <= threshold) {
        sequentialMergeSort(arr, left, right);
        return;
    }

    const int mid = left + (right - left) / 2;
    auto futRight = async(launch::async, [&]() {
        parallelMergeSortAsync(arr, mid + 1, right, threshold);
    });
    parallelMergeSortAsync(arr, left, mid, threshold);
    futRight.get();
    merge(arr, left, mid, right);
}

}  // namespace

int main() {
    constexpr int kN = 50'000;
    vector<int> arr = makeRandomArray(kN, 23);

    cout << "====== Fork-Join std::async (n=" << kN << ", threshold=" << kThreshold
         << ") ======\n";

    const long long ms = timeMs([&]() {
        parallelMergeSortAsync(arr, 0, static_cast<int>(arr.size()) - 1, kThreshold);
    });

    cout << "time=" << ms << " ms | sorted=" << (isSorted(arr) ? "yes" : "NO") << "\n";
    cout << "Head: ";
    printHead(arr);
    cout << "(async may use thread pool internally — OS/library dependent)\n";
    return 0;
}
