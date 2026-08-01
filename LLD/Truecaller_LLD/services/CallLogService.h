// ============================================================================
//  services/CallLogService.h  —  Call history (log karo + recent nikaalo)
// ----------------------------------------------------------------------------
//  Storage: `unordered_map<userId, vector<CallLogEntry>>` — har user ki apni
//  list, insertion order me (purani -> nayi).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛🐛 BUG FIX: "recent calls" me PURANI calls sabse upar aati thi         │
//  │                                                                          │
//  │  PEHLE ye tha:                                                            │
//  │      std::sort(copy.begin(), copy.end(), [](a, b) {                       │
//  │          return a.getTimestampEpochSec() > b.getTimestampEpochSec();      │
//  │      });                                                                  │
//  │                                                                          │
//  │  Dekhne me bilkul sahi lagta hai — descending by timestamp = newest       │
//  │  first. Par ek baat chhoot gayi:                                          │
//  │                                                                          │
//  │  ⭐ TIMESTAMP SECONDS me hai. Ek hi second me hui saari calls ka          │
//  │     timestamp BILKUL SAME hota hai! Aur jab saari keys BARABAR hoti hain, │
//  │     to comparator har jodi ke liye `false` deta hai — yaani sort ke paas  │
//  │     order decide karne ka koi aadhaar hi nahi bachta.                     │
//  │     Upar se `std::sort` STABLE bhi nahi hai (ties ka order guaranteed     │
//  │     nahi). Natija: order effectively insertion order = PURANI PEHLE. 😱   │
//  │                                                                          │
//  │  CONFIRM kiya tha (baseline demo output me hi dikhta hai):                │
//  │      CALL_1 INCOMING ... ts=1784667026                                    │
//  │      CALL_2 MISSED   ... ts=1784667026    <- teeno ka SAME timestamp      │
//  │      CALL_3 OUTGOING ... ts=1784667026                                    │
//  │  "Recent calls" me sabse PURANI call sabse upar. Bilkul ulta. ✅ bug      │
//  │                                                                          │
//  │  FIX (do hisse):                                                          │
//  │   1. `seq_` — ek monotonically badhta hua counter har entry ke saath      │
//  │      (`callCounter_` se aata hai). Ye HAMESHA unique hota hai, chahe      │
//  │      timestamp same ho.                                                  │
//  │   2. Comparator ab TIE-BREAK karta hai: pehle timestamp descending, aur   │
//  │      barabar ho to `seq` descending -> newest call hamesha sabse upar.    │
//  │                                                                          │
//  │  📌 SABAK: jab bhi TIME se sort karo, ek unique tie-breaker rakho.        │
//  │     Milliseconds pe jaane se bhi ye poori tarah nahi jaata — tez system   │
//  │     me same millisecond bhi aa sakta hai. Sequence number pakka hai.      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `getRecentCalls` COPY banata hai (`std::vector<CallLogEntry> copy = ...`)
//     phir usko sort karta hai — asli stored list ko haath nahi lagata.
//     Kyun? Kyunki method `const` hai, aur asli list ka INSERTION ORDER
//     preserve rehna chahiye (wahi to hamara tie-break aadhaar hai). Agar asli
//     ko sort kar dete to wo order kho jaata.
//
//  ⭐ Sort PEHLE, `resize(limit)` BAAD me — warna "latest 5" nahi, random 5
//     milte jo baad me sorted dikhte. (SearchService me bhi yahi gotcha hai.)
//
//  ⚠ Har call pe poori list ki copy + sort = O(N log N). 10 calls pe theek,
//     10000 pe nahi. Asli fix: list ko hamesha newest-first insert karo
//     (front pe daalo / deque) — phir sort ki zaroorat hi nahi, seedha pehle
//     `limit` elements le lo. Chhote LLD me current approach saaf aur kaafi hai.
//
//  ⭐ `nowEpoch()` `static` private helper hai — is class ke bahar kisi kaam ka
//     nahi, aur object ke state se koi lena-dena nahi.
// ============================================================================
#ifndef TRUECALLER_LLD_SERVICES_CALLLOGSERVICE_H
#define TRUECALLER_LLD_SERVICES_CALLLOGSERVICE_H

#include <algorithm>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/CallType.h"
#include "../models/CallLogEntry.h"

namespace truecaller_lld {

class CallLogService {
public:
    std::string logCall(const std::string& userId, const std::string& remotePhone, CallType type) {
        const int seq = ++callCounter_; // ⭐ hamesha unique (tie-breaker)
        const std::string callId = "CALL_" + std::to_string(seq);
        const long long now = nowEpoch();
        // Entry + uska sequence saath me store (insertion order bhi bachta hai).
        entries_[userId].push_back(
            LoggedCall{CallLogEntry(callId, userId, remotePhone, type, now), seq});
        return callId;
    }

    std::vector<CallLogEntry> getRecentCalls(const std::string& userId, size_t limit = 20) const {
        auto it = entries_.find(userId);
        if (it == entries_.end()) {
            return {}; // is user ki koi call nahi (khaali list, error nahi)
        }
        std::vector<LoggedCall> copy = it->second; // ⭐ asli list untouched

        // ⭐⭐ BUG FIX: timestamp descending, aur TIE pe seq descending.
        // Isse ek hi second ki calls bhi hamesha newest-first aati hain.
        std::sort(copy.begin(), copy.end(), [](const LoggedCall& a, const LoggedCall& b) {
            if (a.entry.getTimestampEpochSec() != b.entry.getTimestampEpochSec()) {
                return a.entry.getTimestampEpochSec() > b.entry.getTimestampEpochSec();
            }
            return a.seq > b.seq; // ⭐ same second -> baad wali call pehle
        });

        // ⭐ Sort ke BAAD limit (warna "latest N" nahi milte).
        if (copy.size() > limit) {
            copy.resize(limit);
        }

        std::vector<CallLogEntry> result;
        result.reserve(copy.size());
        for (const LoggedCall& logged : copy) {
            result.push_back(logged.entry); // seq internal hai — bahar nahi jaata
        }
        return result;
    }

private:
    // Entry + uska insertion sequence (sirf sorting ke liye — API me nahi dikhta).
    struct LoggedCall {
        CallLogEntry entry;
        int seq{0};
    };

    std::unordered_map<std::string, std::vector<LoggedCall>> entries_; // userId -> calls
    int callCounter_{0}; // ⭐ callId aur seq dono isi se

    static long long nowEpoch() {
        return std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_SERVICES_CALLLOGSERVICE_H
