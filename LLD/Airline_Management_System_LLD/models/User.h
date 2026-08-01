// ============================================================================
//  models/User.h  —  System me login karne wala (role-based access ka subject)
// ----------------------------------------------------------------------------
//  User = jo system chala raha hai. Har user ka ek ROLE hota hai (UserRole)
//  jo tay karta hai wo kya kar sakta hai (schedule flight? assign crew? bas book?).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `linkedPassengerId` — USER ko PASSENGER se jodta hai                  │
//  │                                                                          │
//  │  Ek PASSENGER role wala user apne passenger record se linked hota hai     │
//  │  (jaise U_PAX -> P1). Admin/staff ke liye ye khaali "" hota hai (unka koi │
//  │  ticket nahi banti, wo bas system operate karte hain).                   │
//  │                                                                          │
//  │  Ye "User (login) vs Passenger (ticket)" wala do-alag-model design hai —  │
//  │  Passenger.h me poora note padho.                                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Facade ka login() active user set karta hai, aur requireRole() har
//     protected action se pehle is user ka role check karta hai. Ye "kaun
//     kya kar sakta hai" wali poori security is ek field (role) pe tiki hai.
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_USER_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_USER_H

#include <string>

#include "../enums/UserRole.h"

namespace airline_mgmt {

struct User {
    std::string userId;            // "U_PAX" — unique (login id)
    std::string name;              // "Passenger User"
    std::string email;             // contact
    UserRole role;                 // PASSENGER / AIRLINE_STAFF / ADMIN (permission)
    std::string linkedPassengerId; // agar PASSENGER hai to uska passenger record ("" warna)
};

} // namespace airline_mgmt

#endif
