// ============================================================================
//  utils/Base62Encoder.h  —  Number ko chhote Base62 code me badlo
// ----------------------------------------------------------------------------
//  Kaam: ek bada number (jaise 1000, 1001...) ko ek chhoti string me convert
//  karna jisme sirf [0-9 a-z A-Z] (kul 62 characters) ho. Isi string ko URL
//  shortener "short code" ki tarah use karta hai (jaise sho.rt/g8).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ BASE 62 KYUN? — kam characters me zyada codes                        │
//  │                                                                          │
//  │  Base 10 (sirf 0-9): 3 digit -> 1000 codes                              │
//  │  Base 62 (0-9,a-z,A-Z): 3 char -> 62^3 = 238,328 codes!                  │
//  │                                                                          │
//  │  Yaani utni hi lambai me BAHUT zyada unique short codes. Isi liye URL     │
//  │  shorteners base62 use karte hain — link chhota rehta hai par crores      │
//  │  URLs handle ho jaate hain. (Slash/plus jaise URL-unsafe chars nahi hain, │
//  │  isliye base64 ke bajaye base62.)                                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ALGORITHM — "number ko base 62 me likho" (jaise base-10 ko base-2)   │
//  │                                                                          │
//  │  Bar-bar 62 se divide karo, har baar ka REMAINDER us position ka digit.  │
//  │     value=125:  125 % 62 = 1  -> char[1]='1',  125/62 = 2                │
//  │                   2 % 62 = 2  -> char[2]='2',    2/62 = 0  (ruk gaye)     │
//  │     remainders ulte mile (1, 2) -> REVERSE karke "21" (base62)            │
//  │  Isi liye aakhir me `reverse()` — kyunki digits peeche se aage bante hain.│
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `static` method — ye ek PURE FUNCTION hai (input number -> output string,
//     koi state nahi). Object banane ki zaroorat nahi: `Base62Encoder::encode(x)`.
//     Isi wajah se ise akele test karna trivial hai (same input = same output).
//
//  📌 value == 0 ka special case: loop `value > 0` pe chalta hai, to 0 ke liye
//     loop chalta hi nahi -> khaali string. Isliye 0 ko alag se "0" return karte.
//     (Waise service counter 1000 se shuru hota, to 0 aata nahi — par safe rehna acchha.)
// ============================================================================
#ifndef URL_SHORTNER_LLD_UTILS_BASE62ENCODER_H
#define URL_SHORTNER_LLD_UTILS_BASE62ENCODER_H

#include <algorithm>
#include <string>

using namespace std;

namespace url_shortner_lld {

class Base62Encoder {
public:
    static string encode(long long value) {
        // 62 characters: index 0-61. Position = base62 "digit".
        const string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        if (value == 0) {
            return "0"; // special case (loop 0 pe nahi chalta — upar note dekho)
        }

        string encoded;
        while (value > 0) {
            encoded.push_back(chars[value % 62]); // remainder = is position ka digit
            value /= 62;                          // agle (bade) digit pe badho
        }
        reverse(encoded.begin(), encoded.end()); // digits ulte bane the -> seedha karo
        return encoded;
    }
};

} // namespace url_shortner_lld

#endif // URL_SHORTNER_LLD_UTILS_BASE62ENCODER_H
