/**
 * 04 — Thread pool + threshold: reuse workers, cap thread churn.
 */
#include <iostream>

#include "MergeSortUtils.h"
#include "SimpleThreadPool.h"

using namespace std;
using namespace merge_sort_demo;

namespace {

constexpr int kThreshold = 256;

void parallelMergeSortThreaded(vector<int>& arr, int left, int right, int threshold);

void parallelMergeSortPool(vector<int>& arr, int left, int right, int threshold,
                           SimpleThreadPool& pool) {
    if (left >= right) {
        return;
    }
    if (right - left + 1 <= threshold) {
        sequentialMergeSort(arr, left, right);
        return;
    }

    const int mid = left + (right - left) / 2;
    // Pool runs ONE half; worker must not submit nested pool tasks (deadlock risk).
    auto futRight = pool.submit(
        [&]() { parallelMergeSortThreaded(arr, mid + 1, right, threshold); });
    parallelMergeSortThreaded(arr, left, mid, threshold);
    futRight.get();
    merge(arr, left, mid, right);
}

void parallelMergeSortThreaded(vector<int>& arr, int left, int right, int threshold) {
    if (left >= right) {
        return;
    }
    if (right - left + 1 <= threshold) {
        sequentialMergeSort(arr, left, right);
        return;
    }
    const int mid = left + (right - left) / 2;
    thread rightThread(parallelMergeSortThreaded, ref(arr), mid + 1, right, threshold);
    parallelMergeSortThreaded(arr, left, mid, threshold);
    rightThread.join();
    merge(arr, left, mid, right);
}

}  // namespace

int main() {
    constexpr int kN = 50'000;
    constexpr size_t kWorkers = 4;

    vector<int> arr = makeRandomArray(kN, 19);
    SimpleThreadPool pool(kWorkers);

    cout << "====== Thread Pool + Threshold (n=" << kN << ", workers=" << kWorkers
         << ", threshold=" << kThreshold << ") ======\n";

    const long long ms = timeMs([&]() {
        parallelMergeSortPool(arr, 0, static_cast<int>(arr.size()) - 1, kThreshold, pool);
    });

    cout << "time=" << ms << " ms | sorted=" << (isSorted(arr) ? "yes" : "NO") << "\n";
    cout << "Head: ";
    printHead(arr);
    return 0;
}
