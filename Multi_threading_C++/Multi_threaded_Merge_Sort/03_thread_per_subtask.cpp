/**
 * 03 — Thread-per-subtask (naive): spawn std::thread for each half until threshold.
 */
#include <iostream>
#include <thread>

#include "MergeSortUtils.h"

using namespace std;
using namespace merge_sort_demo;

namespace {

constexpr int kThreshold = 64;

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
    constexpr int kN = 200;
    vector<int> arr = makeRandomArray(kN, 11);

    cout << "====== Thread-per-Subtask (n=" << kN << ", threshold=" << kThreshold
         << ") ======\n";

    const long long ms = timeMs([&]() {
        parallelMergeSortThreaded(arr, 0, static_cast<int>(arr.size()) - 1, kThreshold);
    });

    cout << "time=" << ms << " ms | sorted=" << (isSorted(arr) ? "yes" : "NO") << "\n";
    cout << "Head: ";
    printHead(arr);
    cout << "(Too small threshold -> too many threads; tune for real workloads)\n";
    return 0;
}
