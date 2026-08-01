// ============================================================================
//  core/IConcurrentMap.h  —  Thread-safe map ka CONTRACT (Strategy interface)
// ----------------------------------------------------------------------------
//  Ye interface batata hai "ek concurrent (thread-safe) map kya-kya karega" —
//  put, get, remove, contains, size. Kaise thread-safety deta hai (ek bada lock
//  ya kai chhote locks), wo har concrete implementation apne tarike se decide karti.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ STRATEGY PATTERN — concurrency ka tareeka pluggable banao             │
//  │                                                                          │
//  │  Do implementations, ek hi interface:                                     │
//  │    - CoarseGrainedHashMap : poore map pe EK mutex (simple, kam parallel)  │
//  │    - StripedHashMap       : N chhote locks (lock striping, zyada parallel)│
//  │                                                                          │
//  │  Client sirf `IConcurrentMap&` pakadta hai — usse pata nahi andar kaunsa  │
//  │  locking chal raha hai. Workload ke hisaab se implementation swap kar do, │
//  │  client code ka ek line nahi badlega. (main.cpp me dono ko ek hi          │
//  │  `demo_basic(IConcurrentMap&)` me pass karte hain — yahi Strategy hai.)   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `std::optional<string>` return (get me) — "mila / nahi mila" bina exception
//     ke. Mila -> value, nahi mila -> std::nullopt. Concurrent map me "miss" ek
//     normal cheez hai (koi aur thread ne abhi delete kiya ho), error nahi.
//
//  ⭐ `strategyName()` — sirf demo/printing ke liye ("kaunsa approach chal raha").
//
//  ⭐ VIRTUAL DESTRUCTOR zaroori: client base pointer/reference se derived object
//     handle karta hai — bina virtual destructor ke cleanup adhoora reh jaata (UB).
// ============================================================================
#ifndef CONCURRENT_HASHMAP_LLD_CORE_ICONCURRENTMAP_H
#define CONCURRENT_HASHMAP_LLD_CORE_ICONCURRENTMAP_H

#include <cstddef>
#include <optional>
#include <string>

namespace concurrent_hashmap_lld {

class IConcurrentMap {
public:
    virtual ~IConcurrentMap() = default;

    virtual void put(const std::string& key, const std::string& value) = 0;   // insert/update
    virtual std::optional<std::string> get(const std::string& key) const = 0; // mila -> value, nahi -> nullopt
    virtual bool remove(const std::string& key) = 0;                          // hataya -> true
    virtual bool contains(const std::string& key) const = 0;                  // hai ya nahi
    virtual size_t size() const = 0;                                          // total entries
    virtual const char* strategyName() const = 0;                            // kaunsa approach (demo ke liye)
};

}  // namespace concurrent_hashmap_lld

#endif  // CONCURRENT_HASHMAP_LLD_CORE_ICONCURRENTMAP_H
