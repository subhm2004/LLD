// ============================================================================
//  enums/UserRole.h  —  Kaun machine use kar raha hai (Requirement R3)
// ----------------------------------------------------------------------------
//  "There are two types of people who can use the system: a regular user and
//   an admin."
//
//     USER  -> aam grahak. Paisa daalta hai, slot chunta hai, product leta hai.
//     ADMIN -> staff/company ka banda. Machine me stock bharta/nikaalta hai (R4).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye enum "PERMISSION" ka faisla karta hai, "identity" ka nahi         │
//  │                                                                          │
//  │  Dhyaan do ki yahan naam/password kuch nahi hai — sirf ROLE hai. Kyun?   │
//  │  Kyunki machine ko ye jaanne ki zaroorat hi nahi ki "ye Ramesh hai ya    │
//  │  Suresh". Usko sirf itna chahiye: "kya isko stock chhune ka HAQ hai?"    │
//  │                                                                          │
//  │  📌 Sabak: system se sirf WAHI cheez poochho jo faisle ke liye ZAROORI   │
//  │     hai. Zyada data maangna = zyada jhanjhat, aur privacy ka risk bhi.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ Ye ek SIMPLIFIED model hai — asli system me yahan proper AUTHENTICATION
//    hoti (admin ko key/password/RFID card se login karna padta). Yahan koi
//    login nahi hai; jo khud ko admin bataye, wo admin hai. Ye jaan-boojh ke
//    simple rakha gaya hai (requirements ki Assumption me bhi likha hai) —
//    focus State pattern aur slots pe hai, security pe nahi.
//
//    Asli system me ye ek `AdminAuthenticator` class hoti jo `UserRole` return
//    karti — aur `AdminPanel` sirf verify hone ke baad hi milta.
// ============================================================================
#ifndef VENDING_MACHINE_LLD_ENUMS_USERROLE_H
#define VENDING_MACHINE_LLD_ENUMS_USERROLE_H

#include <string>

using namespace std;

namespace vending_machine_lld {

enum class UserRole {
    USER, // aam grahak — sirf khareed sakta hai
    ADMIN // staff — stock bhar/nikaal sakta hai (R4)
};

// Enum ko padhne layak string me — printing ke liye.
// (`ItemType.h` me bhi bilkul aisa hi `itemTypeToString` hai — poore codebase
//  me ek jaisa pattern rakhna acchi aadat hai, padhne wala turant samajh jaata.)
inline string userRoleToString(UserRole role) {
    return (role == UserRole::ADMIN) ? "ADMIN" : "USER";
}

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_ENUMS_USERROLE_H
