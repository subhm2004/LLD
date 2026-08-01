// ============================================================================
//  core/ConcurrentHashMap.h  —  "Default" concurrent map ka naam (type alias)
// ----------------------------------------------------------------------------
//  Is repo me DEFAULT concurrent map = StripedHashMap (lock striping). Isi liye
//  ye file ek chhota sa alias deti hai:
//
//      using ConcurrentHashMap = StripedHashMap;
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ALIAS KYUN? — "default choice" ko ek saaf naam de do                  │
//  │                                                                          │
//  │  Client `ConcurrentHashMap` likhe, use itna hi pata ho ki "ye ek accha    │
//  │  thread-safe map hai" — andar striping hai ya kuch aur, wo detail chhupi  │
//  │  rahe. Kal ko default badalna ho (jaise ek nayi ShardedHashMap aaye), to  │
//  │  bas YE EK LINE badlo — poore codebase me `ConcurrentHashMap` likhne wala │
//  │  code apne aap nayi implementation use karne lagega. Ek jagah ka switch.  │
//  │                                                                          │
//  │  Ye Strategy pattern ke saath achha chalta hai: interface (IConcurrentMap)│
//  │  se code likho, aur "default strategy" ka ye alias convenience deta hai.  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  🧹 CLEANUP: pehle yahan ek bekaar `#include <bits/stdc++.h>` tha — is file ko
//     sirf StripedHashMap.h chahiye (baaki poora standard library kheenchne ki
//     zaroorat nahi thi, aur bits/stdc++.h non-portable/GCC-only hai). Hata diya.
// ============================================================================
#ifndef CONCURRENT_HASHMAP_LLD_CORE_CONCURRENTHASHMAP_H
#define CONCURRENT_HASHMAP_LLD_CORE_CONCURRENTHASHMAP_H

#include "StripedHashMap.h"

namespace concurrent_hashmap_lld {

// Default concurrent map = lock striping (fine-grained) — best balance for most workloads.
using ConcurrentHashMap = StripedHashMap;

}  // namespace concurrent_hashmap_lld

#endif  // CONCURRENT_HASHMAP_LLD_CORE_CONCURRENTHASHMAP_H
