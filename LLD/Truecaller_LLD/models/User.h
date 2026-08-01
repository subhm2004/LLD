// ============================================================================
//  models/User.h  —  App me REGISTERED banda (account)
// ----------------------------------------------------------------------------
//  ⚠ Ise `PhoneProfile` se confuse mat karna:
//     User         -> jisne APP install karke account banaya (email, status)
//     PhoneProfile -> KISI BHI number ki public jaankari (register ho ya na ho)
//  Har User ka ek PhoneProfile hota hai, par har PhoneProfile ka User NAHI.
//  (PhoneProfile.h me poora note hai — wo padhna zaroori hai.)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ User MUTABLE hai (setters hain) — PhoneProfile jaisa nahi              │
//  │                                                                          │
//  │  Kyunki ye banda apni khud ki jaankari hai: naam badal sakta hai, email   │
//  │  add kar sakta hai, business account bana sakta hai. Ye sab uska HAQ hai. │
//  │                                                                          │
//  │  ⚠ Par dhyan do: naam badalne pe `TruecallerSystem::updateProfile`        │
//  │     PhoneProfile ko BHI update karta hai — warna directory me purana      │
//  │     naam pada rehta aur lookup galat naam dikhata. Do jagah data hai to   │
//  │     dono ko SAATH me update karna padta hai.                             │
//  │  📌 Ye duplication ki keemat hai. Fayda ye ki lookup ek hi map se O(1)    │
//  │     me ho jaata hai (users_ me jaake join nahi karna padta).             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `phone_` ka koi setter NAHI hai — jaan-boojh ke. Phone hi to identity hai
//     (`phoneToUserId_` ka key). Silently badalne se saara mapping toot jaata:
//     purana key map me pada reh jaata, block list galat number pe lagi rehti.
//     Number badalna ek poora flow hai (OTP verification ke saath), ek setter nahi.
//
//  ⭐ Constructor me `normalizePhone(phone)` — object banega hi tabhi jab number
//     valid ho. Always-valid object (PhoneUtils.h ka note).
//
//  ⭐ `setDisplayName` khaali naam block karta hai — kyunki khaali naam wala
//     profile directory me "Unknown" ban jaata, jo asli unknown number se
//     alag nahi dikhta. Validation setter ke andar = har caller safe.
//
//  📌 `isBusiness_` sirf ek flag hai par iska asar directory tak jaata hai —
//     registration/update pe tag PERSONAL ya BUSINESS set hota hai.
// ============================================================================
#ifndef TRUECALLER_LLD_MODELS_USER_H
#define TRUECALLER_LLD_MODELS_USER_H

#include <stdexcept>  // std::invalid_argument (pehle transitively aata tha — fragile)
#include <string>
#include <utility>

#include "../enums/AccountStatus.h"
#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

class User {
public:
    User() = default;

    User(std::string userId, std::string displayName, std::string phone, std::string email = "",
         bool isBusiness = false)
        : userId_(std::move(userId)),
          displayName_(std::move(displayName)),
          phone_(normalizePhone(phone)), // ⭐ canonical form (always-valid object)
          email_(std::move(email)),
          isBusiness_(isBusiness) {}

    const std::string& getUserId() const { return userId_; }
    const std::string& getDisplayName() const { return displayName_; }
    const std::string& getPhone() const { return phone_; } // ⚠ koi setter nahi (upar note)
    const std::string& getEmail() const { return email_; }
    bool isBusiness() const { return isBusiness_; }
    AccountStatus getStatus() const { return status_; }

    // ⭐ Validation setter ke andar — khaali naam "Unknown" jaisa dikhta hai.
    void setDisplayName(const std::string& name) {
        if (name.empty()) {
            throw std::invalid_argument("display name cannot be empty");
        }
        displayName_ = name;
    }

    void setEmail(const std::string& email) { email_ = email; } // optional field
    void setBusiness(bool value) { isBusiness_ = value; }
    void setStatus(AccountStatus status) { status_ = status; } // ⚠ abhi koi bulata nahi

private:
    std::string userId_;
    std::string displayName_;
    std::string phone_;   // ⭐ identity — immutable
    std::string email_;   // optional
    bool isBusiness_{false};
    AccountStatus status_{AccountStatus::ACTIVE};
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_MODELS_USER_H
