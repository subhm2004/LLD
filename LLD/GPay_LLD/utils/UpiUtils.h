// ============================================================================
//  utils/UpiUtils.h  —  UPI ID validation, phone normalization, QR parsing
// ----------------------------------------------------------------------------
//  Free functions (class nahi) — kyunki inka koi STATE nahi hai. Pure input →
//  output. Aisi cheezon ko class me lapetna bekaar hai. 📌 "Class banao jab
//  state ho; function banao jab sirf calculation ho."
//
//  ⭐ `inline` header me isliye lagaya hai ki ye file kai jagah include hoti hai —
//     bina `inline` ke linker "multiple definition" ka error deta.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `normalizePhone` — duplicate users se bachaata hai                    │
//  │                                                                          │
//  │  User "9876543210" likhe, "+91 98765-43210" likhe ya "091-9876543210" —  │
//  │  teeno ek hi banda hai. Normalize na karte to:                            │
//  │     - phoneToUserId_ map me TEEN alag entries banti                       │
//  │     - ek hi bande ke 3 accounts ban jaate 😱                              │
//  │     - "phone se bhejo" kabhi kaam karta kabhi nahi                        │
//  │                                                                          │
//  │  Logic: pehle SIRF digits nikaalo (spaces/dashes/plus sab uda do), phir   │
//  │  10 digit -> "+91" laga do, 11+ digit -> pehle se country code hai.       │
//  │  ⚠ Ye India-centric hai (hardcoded +91) — ek honest simplification.      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `parseUpiQr` — asli UPI QR ka format                                  │
//  │                                                                          │
//  │      upi://pay?pa=teastall@okbiz&am=120&tn=Chai                          │
//  │                 └payee VPA──┘  └amt┘ └note┘                              │
//  │                                                                          │
//  │  `pa` (payee address) aur `am` (amount) ZAROORI hain, `tn` (note) optional.│
//  │  Parsing ka trick: har field ka `&` tak ka hissa lo. Aakhri field ke      │
//  │  aage `&` nahi hota -> `find` `npos` deta hai -> `substr(start, npos)`    │
//  │  apne aap "string ke end tak" le leta hai. Isliye alag case nahi likha.  │
//  │                                                                          │
//  │  ⭐ QR se aaya VPA bhi `validateUpiId()` se guzarta hai. Trust nothing —  │
//  │     QR koi bhi chhaap sakta hai, wo bahar se aaya hua INPUT hai.         │
//  │  ⚠ Real UPI QR me signature bhi hota hai (`sign=`) jo NPCI verify karta   │
//  │     hai — isse fake QR nahi banaya ja sakta. Demo me wo skip hai.        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `static const std::regex` — regex compile karna MEHNGA hota hai. `static`
//     se wo poore program me sirf EK BAAR banta hai, har call pe nahi.
// ============================================================================
#ifndef GPAY_LLD_UTILS_UPIUTILS_H
#define GPAY_LLD_UTILS_UPIUTILS_H

#include <regex>
#include <stdexcept>
#include <string>

namespace gpay_lld {

// Format: "naam@bank" — naam me letters/digits/. - _ chalte hain, bank sirf letters.
inline bool isValidUpiId(const std::string& upiId) {
    static const std::regex pattern(R"(^[a-zA-Z0-9.\-_]{2,256}@[a-zA-Z]{2,64}$)"); // ⭐ static (ek baar compile)
    return std::regex_match(upiId, pattern);
}

// Throwing version — jahan "valid hona hi chahiye" wahan ye use hota hai.
inline void validateUpiId(const std::string& upiId) {
    if (!isValidUpiId(upiId)) {
        throw std::invalid_argument("invalid UPI ID: " + upiId);
    }
}

// ⭐ Har phone ko EK canonical form me laata hai (upar wala note).
inline std::string normalizePhone(const std::string& raw) {
    std::string digits;
    for (char c : raw) {
        if (c >= '0' && c <= '9') { // spaces, dashes, +, brackets — sab uda do
            digits.push_back(c);
        }
    }
    if (digits.size() == 10) {
        return "+91" + digits; // ⚠ India assume kiya hai
    }
    if (digits.size() >= 11) {
        return "+" + digits;   // country code pehle se hai
    }
    throw std::invalid_argument("invalid phone: " + raw);
}

/** QR se nikla hua data: kisko, kitna, kis note ke saath. */
struct QrPaymentData {
    std::string payeeUpi;
    double amount{0};
    std::string note;
};

// Parse: upi://pay?pa=merchant@okaxis&am=250&tn=Tea
inline QrPaymentData parseUpiQr(const std::string& qrPayload) {
    QrPaymentData data;
    const auto paPos = qrPayload.find("pa=");  // payee address (zaroori)
    const auto amPos = qrPayload.find("am=");  // amount        (zaroori)
    const auto tnPos = qrPayload.find("tn=");  // note          (optional)

    if (paPos == std::string::npos || amPos == std::string::npos) {
        throw std::invalid_argument("invalid UPI QR payload");
    }

    // ⭐ `paEnd` npos ho sakta hai (aakhri field) — substr use "end tak" samajh
    //    leta hai, isliye alag handling ki zaroorat nahi.
    size_t paStart = paPos + 3;
    size_t paEnd = qrPayload.find('&', paStart);
    data.payeeUpi = qrPayload.substr(paStart, paEnd - paStart);
    validateUpiId(data.payeeUpi); // ⭐ QR bahar ka input hai — bharosa mat karo

    size_t amStart = amPos + 3;
    size_t amEnd = qrPayload.find('&', amStart);
    std::string amountStr = qrPayload.substr(amStart, amEnd - amStart);
    data.amount = std::stod(amountStr); // ⚠ galat text -> std::invalid_argument (theek hai)
    if (data.amount <= 0) {
        throw std::invalid_argument("QR amount must be positive");
    }

    if (tnPos != std::string::npos) {
        size_t tnStart = tnPos + 3;
        data.note = qrPayload.substr(tnStart); // note hamesha aakhir me maana hai
    }
    return data;
}

}  // namespace gpay_lld

#endif  // GPAY_LLD_UTILS_UPIUTILS_H
