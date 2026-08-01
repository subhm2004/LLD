/**
 * 06 — Compare sequential vs parallel approaches on same input.
 */
#include <future>
#include <iostream>
#include <thread>

#include "MergeSortUtils.h"
#include "SimpleThreadPool.h"

using namespace std;
using namespace merge_sort_demo;

namespace {

constexpr int kN = 40'000;
constexpr int kThreshold = 512;
constexpr size_t kWorkers = 4;

void parallelThreaded(vector<int>& arr, int left, int right) {
    if (right - left + 1 <= kThreshold) {
        sequentialMergeSort(arr, left, right);
        return;
    }
    const int mid = left + (right - left) / 2;
    thread t2(parallelThreaded, ref(arr), mid + 1, right);
    parallelThreaded(arr, left, mid);
    t2.join();
    merge(arr, left, mid, right);
}

void parallelThreadedInner(vector<int>& arr, int left, int right);

void parallelPool(vector<int>& arr, int left, int right, SimpleThreadPool& pool) {
    if (right - left + 1 <= kThreshold) {
        sequentialMergeSort(arr, left, right);
        return;
    }
    const int mid = left + (right - left) / 2;
    auto f2 = pool.submit([&]() { parallelThreadedInner(arr, mid + 1, right); });
    parallelThreadedInner(arr, left, mid);
    f2.get();
    merge(arr, left, mid, right);
}

void parallelThreadedInner(vector<int>& arr, int left, int right) {
    if (right - left + 1 <= kThreshold) {
        sequentialMergeSort(arr, left, right);
        return;
    }
    const int mid = left + (right - left) / 2;
    thread t2(parallelThreadedInner, ref(arr), mid + 1, right);
    parallelThreadedInner(arr, left, mid);
    t2.join();
    merge(arr, left, mid, right);
}

void parallelAsync(vector<int>& arr, int left, int right) {
    if (right - left + 1 <= kThreshold) {
        sequentialMergeSort(arr, left, right);
        return;
    }
    const int mid = left + (right - left) / 2;
    auto f2 = async(launch::async, [&]() { parallelAsync(arr, mid + 1, right); });
    parallelAsync(arr, left, mid);
    f2.get();
    merge(arr, left, mid, right);
}

}  // namespace

int main() {
    const vector<int> base = makeRandomArray(kN, 99);

    cout << "====== Compare Timings (n=" << kN << ", threshold=" << kThreshold
         << ") ======\n\n";

    vector<int> seq = base;
    const long long seqMs = timeMs([&]() {
        sequentialMergeSort(seq, 0, static_cast<int>(seq.size()) - 1);
    });
    cout << "Sequential:      " << seqMs << " ms | sorted=" << isSorted(seq) << "\n";

    vector<int> th = base;
    const long long thMs = timeMs([&]() {
        parallelThreaded(th, 0, static_cast<int>(th.size()) - 1);
    });
    cout << "Thread/subtask:  " << thMs << " ms | sorted=" << isSorted(th) << "\n";

    vector<int> poolArr = base;
    SimpleThreadPool pool(kWorkers);
    const long long poolMs = timeMs([&]() {
        parallelPool(poolArr, 0, static_cast<int>(poolArr.size()) - 1, pool);
    });
    cout << "Thread pool:     " << poolMs << " ms | sorted=" << isSorted(poolArr) << "\n";

    vector<int> asyncArr = base;
    const long long asyncMs = timeMs([&]() {
        parallelAsync(asyncArr, 0, static_cast<int>(asyncArr.size()) - 1);
    });
    cout << "std::async:      " << asyncMs << " ms | sorted=" << isSorted(asyncArr) << "\n";

    cout << "\n(Speedup depends on cores, overhead, and serial merge — not always faster)\n";
    return 0;
}
