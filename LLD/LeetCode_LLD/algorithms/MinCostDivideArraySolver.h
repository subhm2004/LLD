#ifndef LEETCODE_LLD_ALGORITHMS_MINCOSTDIVIDEARRAYSOLVER_H
#define LEETCODE_LLD_ALGORITHMS_MINCOSTDIVIDEARRAYSOLVER_H

#include <bits/stdc++.h>

namespace leetcode_lld {

using namespace std;

/**
 * Minimum Cost to Divide Array Into Subarrays
 *
 * Original cost of t-th subarray [l..r] (1-based t):
 *   (prefixNums[r] + k * t) * (prefixCost[r] - prefixCost[l])
 *
 * Optimized per-segment cost (equivalent total when summed optimally):
 *   subCost = prefixNums[r] * (cost[l..r]) + k * (cost[l..n-1])
 *
 * DP: dp[i] = min cost to partition nums[i..n-1]
 * Time: O(n^2), Space: O(n)
 */
class MinCostDivideArraySolver {
public:
    static long long solve(const vector<int> &nums, const vector<int> &cost, int k) {
        const int n = static_cast<int>(nums.size());
        if (n == 0 || static_cast<int>(cost.size()) != n) {
            throw invalid_argument("nums and cost must have same non-zero length");
        }

        vector<long long> prefixNums(n + 1, 0);
        vector<long long> prefixCost(n + 1, 0);
        for (int i = 0; i < n; ++i) {
            prefixNums[i + 1] = prefixNums[i] + nums[i];
            prefixCost[i + 1] = prefixCost[i] + cost[i];
        }

        vector<long long> dp(n + 1, -1);
        return solveMemoized(0, prefixNums, prefixCost, dp, k);
    }

private:
    static long long solveMemoized(int startIndex,
                                   const vector<long long> &prefixNums,
                                   const vector<long long> &prefixCost,
                                   vector<long long> &dp,
                                   int k) {
        const int n = static_cast<int>(prefixNums.size()) - 1;
        if (startIndex >= n) {
            return 0;
        }
        if (dp[startIndex] != -1) {
            return dp[startIndex];
        }

        long long minTotal = LLONG_MAX;
        for (int endIndex = startIndex; endIndex < n; ++endIndex) {
            const long long sumNums = prefixNums[endIndex + 1];
            const long long sumCostSubarray =
                prefixCost[endIndex + 1] - prefixCost[startIndex];
            const long long sumCostSuffix =
                prefixCost[n] - prefixCost[startIndex];

            const long long subCost =
                sumNums * sumCostSubarray + static_cast<long long>(k) * sumCostSuffix;

            if (subCost >= minTotal) {
                break;  // pruning
            }

            const long long totalCost =
                subCost + solveMemoized(endIndex + 1, prefixNums, prefixCost, dp, k);
            minTotal = min(minTotal, totalCost);
        }

        return dp[startIndex] = minTotal;
    }
};

}  // namespace leetcode_lld

#endif
