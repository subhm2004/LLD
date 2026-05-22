#ifndef CONCURRENT_HASHMAP_LLD_CORE_ICONCURRENTMAP_H
#define CONCURRENT_HASHMAP_LLD_CORE_ICONCURRENTMAP_H

#include <cstddef>
#include <optional>
#include <string>

namespace concurrent_hashmap_lld {

class IConcurrentMap {
public:
    virtual ~IConcurrentMap() = default;

    virtual void put(const std::string& key, const std::string& value) = 0;
    virtual std::optional<std::string> get(const std::string& key) const = 0;
    virtual bool remove(const std::string& key) = 0;
    virtual bool contains(const std::string& key) const = 0;
    virtual size_t size() const = 0;
    virtual const char* strategyName() const = 0;
};

}  // namespace concurrent_hashmap_lld

#endif  // CONCURRENT_HASHMAP_LLD_CORE_ICONCURRENTMAP_H
