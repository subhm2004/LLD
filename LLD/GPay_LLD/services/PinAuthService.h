// ============================================================================
//  services/PinAuthService.h  —  UPI PIN verification
// ----------------------------------------------------------------------------
//  Chhoti class, par ek BADA principle sikhati hai:
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ Stored PIN kabhi WAPAS NAHI padha jaata — sirf COMPARE hota hai     │
//  │                                                                          │
//  │      hashPin(diya_hua_pin) == user.getPinHash()  ?                       │
//  │                                                                          │
//  │  Yaani hum kabhi ye nahi poochte "user ka PIN kya hai" — hum poochte hain │
//  │  "jo PIN diya gaya, kya wo sahi hai". Farak bahut bada hai:              │
//  │  ek me PIN nikaalna PADTA hai (yaani wo nikaala ja SAKTA hai), doosre me  │
//  │  nikaalne ka koi tareeka hi nahi.                                        │
//  │                                                                          │
//  │  📌 Isi liye hashing "one-way" hoti hai — hash se wapas PIN nahi banta.   │
//  │     Database churaane wale ko sirf hashes milte hain, PIN nahi.          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠⚠ `hashPin` = `"hash_" + pin` — ye BILKUL FAKE hai, sirf pattern dikhane ke
//     liye. Ye REVERSIBLE hai (prefix hata do, PIN mil gaya). Production me:
//       • bcrypt / scrypt / Argon2 — ye jaan-boojh ke SLOW hote hain, taaki
//         brute-force (0000 se 9999 tak try karna) mehnga pade
//       • per-user SALT — taaki do users ka same PIN, alag hash de
//       • asli UPI me to PIN server pe aata hi nahi — device ke Secure Element
//         me rehta hai aur NPCI ke saath encrypted handshake hota hai
//  📌 Interview me ye khud bolna — "demo hash hai, production me salted KDF".
//
//  ⚠ Aur bhi jo yahan MISSING hai (jaan-boojh ke, scope ke liye):
//     • RATE LIMITING / lockout — abhi koi 10000 baar try kar ke PIN tod sakta
//       hai. Asli UPI me 3 galat try = PIN block.
//     • CONSTANT-TIME comparison — `!=` jaldi return kar deta hai jab pehla
//       character mismatch ho. Timing se attacker ko hint mil sakta hai.
//     Ye dono batana interview me plus point hai.
//
//  ⭐ `hashPin` static hai (User banate waqt bhi chahiye hota hai, service object
//     ke bina), aur `verifyPin` non-static — taaki aage chal ke isme attempt
//     counter / lockout wala STATE add kiya ja sake bina API todhe.
// ============================================================================
#ifndef GPAY_LLD_SERVICES_PINAUTHSERVICE_H
#define GPAY_LLD_SERVICES_PINAUTHSERVICE_H

#include <stdexcept>
#include <string>

#include "../models/User.h"

namespace gpay_lld {

class PinAuthService {
public:
    // ⚠ DEMO hash — production me bcrypt/Argon2 + salt (upar note).
    static std::string hashPin(const std::string& pin) { return "hash_" + pin; }

    void verifyPin(const User& user, const std::string& pin) const {
        if (pin.empty()) {
            throw std::invalid_argument("PIN required");
        }
        // ⭐ Stored hash se COMPARE — PIN kabhi nikaala nahi jaata.
        if (hashPin(pin) != user.getPinHash()) {
            throw std::runtime_error("invalid PIN");
        }
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_SERVICES_PINAUTHSERVICE_H
