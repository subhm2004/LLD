// ============================================================================
//  models/User.h  —  Ek registered user (jo ticket book karta hai)
// ----------------------------------------------------------------------------
//  Bahut simple: ek unique id aur naam. IRCTC system users ko id se track karta
//  hai, aur booking/cancel me "ye user valid hai?" check karta hai.
//
//  📌 `struct` (class nahi) — ye ek plain data holder hai (koi logic/invariant
//     nahi jise chhupana pade). Aise DTOs ke liye struct + public fields sahi hai.
//     Ownership/authentication out of scope hai (interview-scope).
// ============================================================================
#ifndef IRCTC_LLD_MODELS_USER_H
#define IRCTC_LLD_MODELS_USER_H

#include <string>

namespace irctc_lld {

struct User {
    std::string userId; // "USR_1" — unique (system generate karta hai)
    std::string name;   // "Alice"
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_MODELS_USER_H
