// ============================================================================
//  enums/UserRole.h  —  Kaun system use kar raha hai (Role-Based Access ka base)
// ----------------------------------------------------------------------------
//  System me 3 tarah ke log hote hain, aur har ek ke apne HAQ (permissions) hain:
//
//     PASSENGER     -> aam grahak. Flight search, book, pay, cancel kar sakta hai.
//     AIRLINE_STAFF -> airline ka staff. Aircraft/crew assign, flight delay.
//     ADMIN         -> sabse upar. Flight schedule, infrastructure add.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ORDER MAAYNE RAKHTA HAI — ye ek "seedhi" (ordinal) hierarchy hai      │
//  │                                                                          │
//  │     PASSENGER = 0,  AIRLINE_STAFF = 1,  ADMIN = 2                        │
//  │                                                                          │
//  │  Facade ka `requireRole()` inhi numbers ko compare karta hai:            │
//  │     "role ka number >= chahiye wale role ka number?"                     │
//  │  Iska matlab UPAR wala role NEECHE ka kaam bhi kar sakta hai — ADMIN (2)  │
//  │  staff (1) ke saare kaam bhi kar sakta hai. Ye jaan-boojh ke hai (asli    │
//  │  duniya me bhi admin ke paas sabse zyada power hoti hai).                 │
//  │                                                                          │
//  │  ⚠ Isi wajah se in constants ka ORDER kabhi mat badalna — warna poori    │
//  │  permission-hierarchy ulti-pulti ho jaayegi.                             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  `enum class` (scoped) hai — `UserRole::ADMIN` likhna padta hai, naam bahar
//  leak nahi hota aur galti se `int` me convert nahi hota. Type safety free.
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_USERROLE_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_USERROLE_H

namespace airline_mgmt {

// ⚠ Order = permission level. PASSENGER(0) < AIRLINE_STAFF(1) < ADMIN(2).
enum class UserRole { PASSENGER, AIRLINE_STAFF, ADMIN };

} // namespace airline_mgmt

#endif
