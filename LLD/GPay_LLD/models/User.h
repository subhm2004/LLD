// ============================================================================
//  models/User.h  —  GPay ka user (phone + UPI ID + PIN hash)
// ----------------------------------------------------------------------------
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ `pinHash_` — PIN kabhi PLAIN TEXT me store NAHI hota                │
//  │                                                                          │
//  │  Field ka naam hi `pinHash_` hai, `pin_` nahi — aur ye jaan-boojh ke hai. │
//  │  Verify karte waqt hum user ka PIN nahi nikaalte; hum diye hue PIN ko     │
//  │  HASH karke stored hash se COMPARE karte hain (PinAuthService dekho).     │
//  │  Iska fayda: database leak ho bhi jaye to kisi ka PIN nahi milta.        │
//  │                                                                          │
//  │  ⚠ Demo me hash function `"hash_" + pin` hai — ye bilkul FAKE hai,       │
//  │     sirf pattern samjhane ke liye. Asli app me: bcrypt / Argon2 with      │
//  │     per-user SALT, aur ideally PIN device ke Secure Element me rehta hai, │
//  │     server pe aata hi nahi.                                              │
//  │  📌 Interview me ye khud bolna — "yahan fake hash hai, production me      │
//  │     salted KDF" — ye batata hai ki tumhe farak pata hai.                 │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Constructor me VALIDATION + NORMALIZATION — dono                      │
//  │                                                                          │
//  │     phone -> `normalizePhone()`  "9876543210" aur "+91 98765-43210"      │
//  │                                  dono ek hi "+919876543210" ban jaate    │
//  │     upiId -> `validateUpiId()`   format galat -> object banega hi NAHI    │
//  │                                                                          │
//  │  ⭐ Ye "ALWAYS-VALID OBJECT" ka principle hai: agar User object exist     │
//  │     karta hai to uska data GUARANTEED valid hai. Baaki poore system me    │
//  │     phir kabhi `if (isValidUpi(...))` likhne ki zaroorat nahi.           │
//  │  ⭐ Normalization ka fayda: phone lookup map me sirf EK format aata hai,  │
//  │     isliye "+91 98765 43210" se search karo ya "9876543210" se — dono     │
//  │     kaam karenge. Bina normalize kiye duplicate users ban jaate! 😱      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Saare getters, koi setter nahi — User IMMUTABLE hai. Phone/UPI badalna ho
//     to wo ek alag business flow hai (re-verification ke saath), silent setter
//     se nahi. Immutable objects se aadha bug apne aap khatam ho jaata hai.
// ============================================================================
#ifndef GPAY_LLD_MODELS_USER_H
#define GPAY_LLD_MODELS_USER_H

#include <string>
#include <utility>

#include "../utils/UpiUtils.h"

namespace gpay_lld {

class User {
public:
    User() = default;

    User(std::string userId, std::string name, std::string phone, std::string upiId,
         std::string pinHash)
        : userId_(std::move(userId)),
          name_(std::move(name)),
          phone_(normalizePhone(phone)),  // ⭐ "9876543210" -> "+919876543210"
          upiId_(std::move(upiId)),
          pinHash_(std::move(pinHash)) {  // ⭐ HASH, plain PIN nahi
        validateUpiId(upiId_);            // ⭐ galat format -> object banta hi nahi
    }

    const std::string& getUserId() const { return userId_; }
    const std::string& getName() const { return name_; }
    const std::string& getPhone() const { return phone_; } // hamesha normalized
    const std::string& getUpiId() const { return upiId_; }
    const std::string& getPinHash() const { return pinHash_; }
    // ⚠ koi setter nahi — User immutable hai (upar note)

private:
    std::string userId_;
    std::string name_;
    std::string phone_;   // normalized "+91..."
    std::string upiId_;   // validated "name@bank"
    std::string pinHash_; // ⭐ kabhi plain PIN nahi
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_MODELS_USER_H
