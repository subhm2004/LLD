#ifndef CONCURRENT_HASHMAP_LLD_CORE_CONCURRENTHASHMAP_H
#define CONCURRENT_HASHMAP_LLD_CORE_CONCURRENTHASHMAP_H

/**
 * Default concurrent map for this LLD — lock striping (fine-grained).
 * C++17: std::unordered_map per stripe + std::mutex per stripe.
 */
#include "StripedHashMap.h"
#include <bits/stdc++.h>

namespace concurrent_hashmap_lld {

using ConcurrentHashMap = StripedHashMap;

} // namespace concurrent_hashmap_lld

#endif // CONCURRENT_HASHMAP_LLD_CORE_CONCURRENTHASHMAP_H
