// ============================================================================
//  services/DailyLimitService.h  —  Roz ki UPI transfer limit (RBI wala rule)
// ----------------------------------------------------------------------------
//  Asli UPI me roz ka ek limit hota hai (₹1 lakh). Ye service wahi track karti
//  hai: kis user ne aaj kitna bhej diya.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛 BUG FIX: FAILED transactions bhi limit kha jaate the                  │
//  │                                                                          │
//  │  PEHLE sirf `recordSpend()` tha — koi "wapas karo" wala method hi nahi.   │
//  │  Aur TransferService `recordSpend` PAYMENT SE PEHLE bulata hai (sahi hai —│
//  │  limit ek PRE-CHECK hai). Par payment fail ho jaye to?                    │
//  │  ...kuch nahi hota tha. Limit kata hi reh jaata tha. 😱                   │
//  │                                                                          │
//  │  CONFIRM kiya tha: ₹10 balance wale user ne 21 baar ₹5000 bhejne ki       │
//  │  koshish ki — SAARE fail hue (insufficient balance), EK RUPAYA nahi gaya. │
//  │  Phir bhi 21vi baar error aaya: "daily UPI limit exceeded". Yaani user ka │
//  │  poora ₹1 lakh ka limit khatam — bina ek paisa bheje!                     │
//  │                                                                          │
//  │  FIX: `releaseSpend()` add kiya, aur TransferService har failure path pe  │
//  │  use bulata hai. Ab sirf SUCCESSFUL transfers hi limit khaate hain.       │
//  │                                                                          │
//  │  📌 Ye bilkul wahi pattern hai jo inventory reservation me hota hai:      │
//  │     "pehle hold karo → kaam hua to rakho, fail hua to CHHOD do".         │
//  │     Jab bhi kuch kaam se PEHLE reserve karo, uska release bhi likho.     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `recordSpend` CHECK aur RECORD dono ek saath karta hai — ye jaan-boojh ke
//     hai. Alag `canSpend()` + `record()` hote to beech me race condition ka
//     darwaza khul jaata (do threads dono "haan ho sakta hai" sun lete, phir
//     dono record kar dete = limit paar). Ek hi atomic-ish step behtar hai.
//     📌 Ye "check-then-act" anti-pattern se bachne ka tareeka hai.
//
//  ⚠ Naam `spentToday_` hai par ye kabhi RESET nahi hota — koi date/clock hai
//     hi nahi is demo me. Real me: `unordered_map<userId, pair<date, amount>>`
//     aur din badle to counter zero, ya Redis me TTL wali key jo aadhi raat ko
//     apne aap mit jaye. Honest simplification hai.
//
//  ⚠ `releaseSpend` me `std::max(0.0, ...)` clamp hai — taaki double ki thodi si
//     rounding error ya galat call se counter NEGATIVE na ho jaye (warna user ko
//     limit se ZYADA bhejne ka mauka mil jaata).
// ============================================================================
#ifndef GPAY_LLD_SERVICES_DAILYLIMITSERVICE_H
#define GPAY_LLD_SERVICES_DAILYLIMITSERVICE_H

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace gpay_lld {

class DailyLimitService {
public:
    // Limit injectable hai (hard-coded nahi) — testing me chhoti limit de sakte ho.
    explicit DailyLimitService(double dailyLimitInr = 100000.0) : dailyLimit_(dailyLimitInr) {}

    // ⭐ CHECK + RECORD ek hi step me (race se bachne ke liye — upar note).
    void recordSpend(const std::string& userId, double amount) {
        const double next = spentToday_[userId] + amount;
        if (next > dailyLimit_) {
            throw std::runtime_error("daily UPI limit exceeded");
        }
        spentToday_[userId] = next;
    }

    // ⭐ BUG FIX: transfer fail ho to kata hua limit WAPAS karo (upar note).
    // Isko bulaana TransferService ki zimmedari hai — har failure path pe.
    void releaseSpend(const std::string& userId, double amount) {
        auto it = spentToday_.find(userId);
        if (it == spentToday_.end()) {
            return; // kuch record hi nahi hua -> no-op (idempotent-ish, safe)
        }
        // clamp: kabhi negative na ho (warna limit se zyada bhej pate) ⚠
        it->second = std::max(0.0, it->second - amount);
    }

    double getRemaining(const std::string& userId) const {
        auto it = spentToday_.find(userId);
        if (it == spentToday_.end()) {
            return dailyLimit_; // aaj kuch bheja hi nahi -> poora limit bacha hai
        }
        return dailyLimit_ - it->second;
    }

private:
    double dailyLimit_;
    std::unordered_map<std::string, double> spentToday_; // ⚠ kabhi reset nahi hota (upar note)
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_SERVICES_DAILYLIMITSERVICE_H
