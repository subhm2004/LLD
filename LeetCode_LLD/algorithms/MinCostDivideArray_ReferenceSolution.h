#ifndef LEETCODE_LLD_ALGORITHMS_MINCOSTDIVIDEARRAY_REFERENCESOLUTION_H
#define LEETCODE_LLD_ALGORITHMS_MINCOSTDIVIDEARRAY_REFERENCESOLUTION_H

#include <bits/stdc++.h>

/**
 * Reference LeetCode-style Solution (user-provided approach).
 * Used for study / interview revision — judge uses MinCostDivideArraySolver.h (same logic).
 *
 * Variants in this file:
 *   1. solveRE          — recursion with subarray_num, O(n^2) states
 *   2. solveME          — memo on (i, subarray_num)
 *   3. solveRE_Optimized — O(n^2) with transformed cost formula
 *   4. solveME_Optimized — memo on i only (production approach)
 */
class MinCostDivideArrayReferenceSolution {
public:
    long long minimumCost(vector<int> &nums, vector<int> &cost, int k) {
        const int n = static_cast<int>(nums.size());
        vector<long long> prefixNums(n + 1, 0);
        vector<long long> prefixCost(n + 1, 0);
        for (int i = 0; i < n; ++i) {
            prefixNums[i + 1] = prefixNums[i] + nums[i];
            prefixCost[i + 1] = prefixCost[i] + cost[i];
        }

        vector<long long> dp(n + 1, -1);
        return solveME_Optimized(0, prefixNums, prefixCost, dp, k);
    }

private:
    long long solveRE(int i,
                      int subarrayNum,
                      vector<int> &nums,
                      vector<int> &cost,
                      vector<long long> &prefixNums,
                      vector<long long> &prefixCost,
                      int k) {
        const int n = static_cast<int>(nums.size());
        if (i >= n) {
            return 0;
        }

        long long minTotal = LLONG_MAX;
        for (int j = i; j < n; ++j) {
            const long long sumNums = prefixNums[j + 1];
            const long long sumCost = prefixCost[j + 1] - prefixCost[i];
            const long long subarrayCost =
                (sumNums + static_cast<long long>(k) * subarrayNum) * sumCost;

            if (subarrayCost >= minTotal) {
                break;
            }

            const long long totalCost =
                subarrayCost +
                solveRE(j + 1, subarrayNum + 1, nums, cost, prefixNums, prefixCost, k);
            minTotal = min(minTotal, totalCost);
        }
        return minTotal;
    }

    long long solveME(int i,
                      int subarrayNum,
                      vector<int> &nums,
                      vector<int> &cost,
                      vector<long long> &prefixNums,
                      vector<long long> &prefixCost,
                      vector<vector<long long>> &dp2d,
                      int k) {
        const int n = static_cast<int>(nums.size());
        if (i >= n) {
            return 0;
        }
        if (dp2d[i][subarrayNum] != -1) {
            return dp2d[i][subarrayNum];
        }

        long long minTotal = LLONG_MAX;
        for (int j = i; j < n; ++j) {
            const long long sumNums = prefixNums[j + 1];
            const long long sumCost = prefixCost[j + 1] - prefixCost[i];
            const long long subarrayCost =
                (sumNums + static_cast<long long>(k) * subarrayNum) * sumCost;

            if (subarrayCost >= minTotal) {
                break;
            }

            const long long totalCost =
                subarrayCost +
                solveME(j + 1, subarrayNum + 1, nums, cost, prefixNums, prefixCost, dp2d, k);
            minTotal = min(minTotal, totalCost);
        }
        return dp2d[i][subarrayNum] = minTotal;
    }

    long long solveRE_Optimized(int i,
                                vector<long long> &prefixNums,
                                vector<long long> &prefixCost,
                                int k) {
        const int n = static_cast<int>(prefixNums.size()) - 1;
        if (i >= n) {
            return 0;
        }

        long long minTotal = LLONG_MAX;
        for (int j = i; j < n; ++j) {
            const long long sumNums = prefixNums[j + 1];
            const long long sumCostLr = prefixCost[j + 1] - prefixCost[i];
            const long long sumCostLn = prefixCost[n] - prefixCost[i];
            const long long subCost = sumNums * sumCostLr + static_cast<long long>(k) * sumCostLn;

            if (subCost >= minTotal) {
                break;
            }

            const long long totalCost =
                subCost + solveRE_Optimized(j + 1, prefixNums, prefixCost, k);
            minTotal = min(minTotal, totalCost);
        }
        return minTotal;
    }

    long long solveME_Optimized(int i,
                                vector<long long> &prefixNums,
                                vector<long long> &prefixCost,
                                vector<long long> &dp,
                                int k) {
        const int n = static_cast<int>(prefixNums.size()) - 1;
        if (i >= n) {
            return 0;
        }
        if (dp[i] != -1) {
            return dp[i];
        }

        long long minTotal = LLONG_MAX;
        for (int j = i; j < n; ++j) {
            const long long sumNums = prefixNums[j + 1];
            const long long sumCostLr = prefixCost[j + 1] - prefixCost[i];
            const long long sumCostLn = prefixCost[n] - prefixCost[i];
            const long long subCost = sumNums * sumCostLr + static_cast<long long>(k) * sumCostLn;

            if (subCost >= minTotal) {
                break;
            }

            const long long totalCost =
                subCost + solveME_Optimized(j + 1, prefixNums, prefixCost, dp, k);
            minTotal = min(minTotal, totalCost);
        }
        return dp[i] = minTotal;
    }
};

#endif
