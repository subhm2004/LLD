// ============================================================================
//  services/SearchService.h  —  Directory search (naam prefix / exact number)
// ----------------------------------------------------------------------------
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Do search bilkul alag kism ki hain                                    │
//  │                                                                          │
//  │     searchByPhone -> EXACT match, O(1) hash lookup, 0 ya 1 result         │
//  │     searchByName  -> PREFIX match, poora scan, kai results                │
//  │                                                                          │
//  │  Farak kyun? Phone directory ka KEY hai — seedha `find()`. Naam key nahi  │
//  │  hai, aur prefix match hash se ho hi nahi sakta (hash exact match ke      │
//  │  liye hota hai, "shuru me ye aata hai" ke liye nahi).                    │
//  │  📌 Isliye ek O(1) hai aur doosra O(N) — data structure ne decide kiya.  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛 BUG FIX: search results ARBITRARY aur NON-DETERMINISTIC the           │
//  │                                                                          │
//  │  PEHLE ye tha:                                                            │
//  │      for (const auto& entry : directory) {        // unordered_map!      │
//  │          if (match) { matches.push_back(...);                            │
//  │                       if (matches.size() >= limit) break; }  // ⚠        │
//  │      }                                                                    │
//  │                                                                          │
//  │  Do problem:                                                              │
//  │   1. `unordered_map` ka traversal order koi guarantee nahi deta. Results  │
//  │      bilkul random order me aate the.                                    │
//  │   2. `break` ka matlab: limit=2 pe jo do sabse PEHLE scan me mile, wahi   │
//  │      mile — koi ranking nahi. Yaani "top 2" ARBITRARY 2 the.             │
//  │                                                                          │
//  │  CONFIRM kiya tha: Alpha/Bravo/Charlie/Delta/Echo/Foxtrot register kiye,  │
//  │  "Aman" search kiya -> result aaya: Foxtrot Echo Delta Charlie Bravo      │
//  │  Alpha (ULTA!). Aur limit=2 -> [Foxtrot] [Echo]. 😱                       │
//  │  Asli app me iska matlab: same search, alag-alag natije — user confuse,   │
//  │  aur tests flaky.                                                        │
//  │                                                                          │
//  │  FIX: pehle SAARE matches collect karo, phir SORT karo (naam, phir phone  │
//  │  tie-break), TAB limit lagao. Ab result hamesha deterministic aur         │
//  │  alphabetical hai.                                                       │
//  │  ⭐ ORDER MATTERS: sort PEHLE, truncate BAAD me. Ulta karte (jo pehle     │
//  │     mile unhe sort karo) to phir bhi arbitrary 2 hi milte — bas sorted    │
//  │     dikhte. Ye ek classic "sort-then-limit vs limit-then-sort" gotcha hai.│
//  │                                                                          │
//  │  ⚠ Keemat: ab hamesha poora scan hota hai (pehle limit pe ruk jaata).     │
//  │     Chhoti directory pe farak nahi. Asli fix: naam ka TRIE ya prefix      │
//  │     index banao -> O(prefix + k), aur ranking (popularity/contacts) se    │
//  │     sort karo. Ye HLD ka scope hai.                                      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `name.rfind(key, 0) == 0` — ye "starts with" ka C++17 idiom hai.
//     `rfind(key, 0)` ka matlab: "position 0 se PEECHE ki taraf dhundo" —
//     yaani sirf position 0 pe check hota hai. Mila to 0 return, warna npos.
//     `substr(0, key.size()) == key` se behtar hai kyunki isme koi temporary
//     string nahi banti. (C++20 me seedha `name.starts_with(key)` hai.)
//
//  ⭐ Dono taraf `toLower` — query aur stored naam. Warna "ali" se "Alice"
//     kabhi match nahi karta. Case-insensitive search me DONO side normalize
//     karna padta hai — ye wahi soch hai jo phone normalization me thi.
//
//  ⭐ `searchByPhone` vector return karta hai (single profile nahi) — taaki
//     dono search methods ka RETURN TYPE same rahe. UI ek hi tarah se dono ka
//     result render kar sakta hai. Consistent API > "optimized" API.
// ============================================================================
#ifndef TRUECALLER_LLD_SERVICES_SEARCHSERVICE_H
#define TRUECALLER_LLD_SERVICES_SEARCHSERVICE_H

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/PhoneProfile.h"
#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

class SearchService {
public:
    std::vector<PhoneProfile> searchByNamePrefix(
        const std::string& prefix,
        const std::unordered_map<std::string, PhoneProfile>& directory,
        size_t limit = 10) const {
        const std::string key = toLower(prefix); // ⭐ query side normalize
        std::vector<PhoneProfile> matches;

        // ---- STEP 1: SAARE matches collect karo (ab beech me break nahi) ----
        for (const auto& entry : directory) {
            const PhoneProfile& profile = entry.second;
            if (profile.getDisplayName().empty()) {
                continue; // bina naam wale profiles (sirf reported) skip
            }
            const std::string name = toLower(profile.getDisplayName()); // ⭐ stored side normalize
            if (name.rfind(key, 0) == 0) { // ⭐ "starts with" idiom (upar note)
                matches.push_back(profile);
            }
        }

        // ---- STEP 2: ⭐ SORT (deterministic order) — BUG FIX ---------------
        // Naam se sort, same naam ho to phone se tie-break (taaki bilkul
        // deterministic ho — do "Aman Kumar" bhi hamesha same order me aayein).
        std::sort(matches.begin(), matches.end(),
                  [](const PhoneProfile& a, const PhoneProfile& b) {
                      const std::string an = toLower(a.getDisplayName());
                      const std::string bn = toLower(b.getDisplayName());
                      if (an != bn) {
                          return an < bn;
                      }
                      return a.getPhone() < b.getPhone();
                  });

        // ---- STEP 3: TAB limit lagao (sort ke BAAD — order matters!) -------
        if (matches.size() > limit) {
            matches.resize(limit);
        }
        return matches;
    }

    // Exact match — O(1), 0 ya 1 result.
    std::vector<PhoneProfile> searchByPhone(
        const std::string& phone,
        const std::unordered_map<std::string, PhoneProfile>& directory) const {
        const std::string normalized = normalizePhone(phone); // ⭐ key ka format
        auto it = directory.find(normalized);
        if (it == directory.end()) {
            return {}; // khaali vector (nullptr/throw nahi — "nahi mila" normal hai)
        }
        return {it->second}; // ⭐ vector hi (API consistency — upar note)
    }
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_SERVICES_SEARCHSERVICE_H
