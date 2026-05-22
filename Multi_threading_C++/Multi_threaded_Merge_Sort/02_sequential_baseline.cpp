/**
 * 02 — Sequential merge sort baseline.
 */
#include <iostream>

#include "MergeSortUtils.h"

using namespace std;
using namespace merge_sort_demo;

int main() {
    constexpr int kN = 20;
    vector<int> arr = makeRandomArray(kN, 7);

    cout << "====== Sequential Merge Sort (n=" << kN << ") ======\n";
    cout << "Before: ";
    printHead(arr, kN);

    sequentialMergeSort(arr, 0, static_cast<int>(arr.size()) - 1);

    cout << "After:  ";
    printHead(arr, kN);
    cout << "sorted=" << (isSorted(arr) ? "yes" : "NO") << "\n";
    return 0;
}
