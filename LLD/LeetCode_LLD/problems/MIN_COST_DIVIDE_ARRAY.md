# Minimum Cost to Divide Array Into Subarrays (Hard)

You are given two integer arrays, `nums` and `cost`, of the same size, and an integer `k`.

You can divide `nums` into subarrays. The cost of the **i-th** subarray consisting of elements `nums[l..r]` is:

```
(nums[0] + nums[1] + ... + nums[r] + k * i) * (cost[l] + cost[l + 1] + ... + cost[r])
```

`i` is **1-based** subarray order (first subarray = 1, second = 2, …).

Return the **minimum total cost** over all valid divisions.

## Example 1

**Input:** `nums = [3,1,4]`, `cost = [4,6,6]`, `k = 1`  
**Output:** `110`

**Explanation:** Split into `[3,1]` and `[4]`.

- Subarray 1: `(3 + 1 + 1*1) * (4 + 6) = 50`
- Subarray 2: `(3 + 1 + 4 + 1*2) * 6 = 60`

## Example 2

**Input:** `nums = [4,8,5,1,14,2,2,12,1]`, `cost = [7,2,8,4,2,2,1,1,2]`, `k = 7`  
**Output:** `985`

## Constraints

- `1 <= nums.length <= 1000`
- `cost.length == nums.length`
- `1 <= nums[i], cost[i] <= 1000`
- `1 <= k <= 1000`

## Approach (interview)

### Brute — `dp[i][t]` — O(n³)

Track subarray count `t` explicitly. Correct but slow for `n = 1000`.

### Optimized — `dp[i]` — O(n²) ✅ (used in repo)

Transform cost of subarray `[i..j]`:

```
subCost = prefixNums[j+1] * (prefixCost[j+1] - prefixCost[i])
        + k * (prefixCost[n] - prefixCost[i])
```

```
dp[i] = min over j >= i:
    subCost(i, j) + dp[j+1]
```

**Pruning:** if `subCost >= minTotal`, break inner loop.

Reference with all 4 variants: `algorithms/MinCostDivideArray_ReferenceSolution.h`
