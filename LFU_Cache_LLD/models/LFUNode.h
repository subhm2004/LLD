#ifndef LFU_CACHE_LLD_MODELS_LFUNODE_H
#define LFU_CACHE_LLD_MODELS_LFUNODE_H

#include <list>
#include <utility>

#include <bits/stdc++.h>

namespace lfu_cache_lld {

template <typename Key, typename Value>
struct LFUNode {
    using BucketList = std::list<LFUNode<Key, Value> *>;

    Key key{};
    Value value{};
    int frequency{0};
    typename BucketList::iterator bucketIterator{};

    LFUNode() = default;

    LFUNode(Key keyValue, Value valueData)
        : key(std::move(keyValue)), value(std::move(valueData)) {}
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_MODELS_LFUNODE_H
