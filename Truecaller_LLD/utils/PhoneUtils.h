// ============================================================================
//  utils/PhoneUtils.h  —  ⭐ Phone normalization (POORE SYSTEM KI NEEV)
// ----------------------------------------------------------------------------
//  Ye chhoti si file is LLD ka sabse zaroori hissa hai. Kyun? Kyunki phone
//  number yahan HAR JAGAH ka KEY hai:
//
//     directory_[phone]         -> global profile
//     phoneToUserId_[phone]     -> registration lookup
//     blocked_[user].count(phone) -> block check
//     ContactEntry / CallLogEntry -> dono phone se jude
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ Agar normalization NA hoti to poora system silently TOOT jaata      │
//  │                                                                          │
//  │  Ek hi number ke roop:                                                   │
//  │      "9876543210"  "+91 9876543210"  "098765-43210"  "(+91) 98765 43210" │
//  │                                                                          │
//  │  Bina normalize kiye ye CHAAR ALAG KEYS hain. Natija:                    │
//  │   • User ne "+91 98765 43210" block kiya, call "9876543210" se aayi ->    │
//  │     block kaam hi nahi karega 😱                                          │
//  │   • Spam reports 4 alag profiles me bat jaate -> koi bhi 70 tak nahi      │
//  │     pahunchta -> spammer kabhi flag hi nahi hota 😱                       │
//  │   • Ek hi banda 4 baar register kar leta (unique check bypass) 😱         │
//  │                                                                          │
//  │  Isi liye normalization CONSTRUCTOR ke andar hai (User, PhoneProfile,     │
//  │  ContactEntry, CallLogEntry — sab me). Yaani galat format wala object     │
//  │  BAN HI NAHI SAKTA. Har service me `if (normalize...)` likhne ki zaroorat │
//  │  nahi padti — ye "ALWAYS-VALID OBJECT" ka principle hai.                 │
//  │                                                                          │
//  │  📌 Rule: jo cheez KEY banegi, use ek hi CANONICAL FORM me lao — aur ye   │
//  │     kaam boundary pe (object banate waqt) karo, andar nahi.              │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  Logic: sirf digits nikaalo (spaces/dashes/brackets/plus sab uda do), phir
//     10 digit -> "+91" laga do (India assume)
//     11+      -> "+" laga do (country code pehle se hai)
//     <10      -> throw (galat number)
//  ⚠ Ye India-centric hai — asli app libphonenumber (Google) use karti hai jo
//     har country ka format, area code, aur validity jaanti hai.
//
//  ⭐ `toLower` — case-insensitive naam search ke liye. Search me dono taraf
//     (query aur stored naam) lowercase karke compare hota hai, warna "ali"
//     se "Alice" kabhi match nahi karta.
//
//  📌 `static_cast<unsigned char>` — `std::isdigit`/`std::tolower` ko negative
//     value dena UNDEFINED BEHAVIOR hai (aur `char` kai platforms pe signed hota
//     hai). Non-ASCII byte (jaise accented character) aa jaye to seedha `c`
//     pass karna crash/garbage de sakta hai. Ye cast us bug se bachata hai —
//     chhota sa lagta hai par ye ek classic C++ gotcha hai.
// ============================================================================
#ifndef TRUECALLER_LLD_UTILS_PHONEUTILS_H
#define TRUECALLER_LLD_UTILS_PHONEUTILS_H

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>

namespace truecaller_lld {

// ⭐ Har phone ko EK canonical form me laata hai (upar wala note).
inline std::string normalizePhone(const std::string& raw) {
    if (raw.empty()) {
        throw std::invalid_argument("phone number cannot be empty");
    }
    std::string digits;
    for (char c : raw) {
        // ⭐ unsigned char cast zaroori hai (upar note)
        if (std::isdigit(static_cast<unsigned char>(c)) != 0) {
            digits.push_back(c); // spaces, +, -, () — sab chhod do
        }
    }
    if (digits.size() < 10) {
        throw std::invalid_argument("invalid phone number: " + raw);
    }
    if (digits.size() == 10) {
        return "+91" + digits; // ⚠ India assume kiya hai
    }
    return "+" + digits;       // country code pehle se hai
}

// Case-insensitive search ke liye (by-value leta hai — copy pe hi kaam karta hai).
inline std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_UTILS_PHONEUTILS_H
