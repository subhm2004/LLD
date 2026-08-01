// ============================================================================
//  services/AuthenticationService.h  —  Card + PIN verify karo (Requirement #1)
// ----------------------------------------------------------------------------
//  Iska ek hi kaam: diye gaye card ke liye PIN sahi hai ya nahi, ye batao.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Itni chhoti class ALAG kyun? — SINGLE RESPONSIBILITY + future-proof   │
//  │                                                                          │
//  │  Abhi ye bas card.validatePin() ko forward karti hai (one-liner). Phir   │
//  │  bhi alag class kyun? Kyunki AUTHENTICATION ek badalne wala concern hai:  │
//  │     - kal ko "3 galat PIN pe card block" logic add hoga                   │
//  │     - ya OTP / biometric / 2FA aayega                                     │
//  │     - ya PIN hashing/encryption lagega                                    │
//  │  Ye sab is ek jagah aayega — ATMSystem (facade) ko chhue bina. Auth ka    │
//  │  logic ek dedicated jagah rehta hai. Yahi SRP ka fayda hai.              │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Notice: ATMSystem khud PIN compare nahi karta — wo is service ko DELEGATE
//     karta hai. Facade "kya karna hai" jaanta hai, "kaise" services jaanti hain.
// ============================================================================
#ifndef ATM_LLD_SERVICES_AUTHENTICATIONSERVICE_H
#define ATM_LLD_SERVICES_AUTHENTICATIONSERVICE_H

#include <string>

#include "../models/Card.h"

using namespace std;

namespace atm_lld {

class AuthenticationService {
public:
    // Card ka PIN entered PIN se match karta hai? (abhi simple; future me yahi
    // jagah retry-limit / hashing / OTP ke liye extend hogi.)
    bool authenticate(const Card &card, const string &enteredPin) const {
        return card.validatePin(enteredPin);
    }
};

} // namespace atm_lld

#endif // ATM_LLD_SERVICES_AUTHENTICATIONSERVICE_H
