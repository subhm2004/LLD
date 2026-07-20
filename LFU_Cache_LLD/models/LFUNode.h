// ============================================================================
//  models/LFUNode.h  —  LFU cache ka ek entry (node)
// ----------------------------------------------------------------------------
//  Har cached item ek LFUNode hota hai. Isme 4 cheezein:
//     key        -> lookup ke liye
//     value      -> asli data
//     frequency  -> ye item kitni baar access hua (LFU ka dil)
//     bucketIterator -> is node ki apni frequency-list me EXACT jagah ka pata
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `bucketIterator` — sabse important field, isi se O(1) milta hai       │
//  │                                                                          │
//  │  Node kisi frequency-bucket (std::list<Node*>) me pada hota hai. Jab      │
//  │  frequency badhti hai, node ko us list se HATANA padta hai. Bina iske,    │
//  │  list ko poora scan karke node dhoondhna padta (O(n)). Par node apni      │
//  │  jagah ka iterator KHUD paas rakhta hai -> list.erase(iterator) = O(1)!   │
//  │                                                                          │
//  │  💡 Ye "node apna position-iterator store kare" wali trick LRU/LFU dono   │
//  │     ka core hai — isi se average O(1) get/put milta hai.                 │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `BucketList = std::list<LFUNode*>` — list RAW pointers rakhti hai (own
//     nahi karti). Asli node ka maalik keyMap_ (unique_ptr) hai. (LFUCacheCore.h
//     me ownership wala poora note padho.)
//
//  ⭐ `{}` (brace init) har field pe — key/value default-constructed, frequency 0,
//     iterator "singular" (default). Uninitialized garbage ka koi risk nahi.
// ============================================================================
#ifndef LFU_CACHE_LLD_MODELS_LFUNODE_H
#define LFU_CACHE_LLD_MODELS_LFUNODE_H

#include <list>
#include <utility>

namespace lfu_cache_lld {

template <typename Key, typename Value>
struct LFUNode {
    using BucketList = std::list<LFUNode<Key, Value> *>; // freq-bucket ka type (raw ptrs)

    Key key{};                                    // lookup key
    Value value{};                                // asli data
    int frequency{0};                             // kitni baar access hua (LFU signal)
    typename BucketList::iterator bucketIterator{}; // apni list me jagah ka pata (O(1) erase)

    LFUNode() = default;

    // key/value ko move karke set (copy bachti hai — bade values ke liye faydemand)
    LFUNode(Key keyValue, Value valueData)
        : key(std::move(keyValue)), value(std::move(valueData)) {}
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_MODELS_LFUNODE_H
