// ============================================================================
//  enums/UserRole.h — User ke 2 roles (requirement #1) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #1: system me 2 tarah ke users hain:
//    CUSTOMER     -> jo vehicles rent karta hai
//    RECEPTIONIST -> jo rental process manage karta hai (staff)
//
//  User base class ka polymorphic type identify karne ke liye. Customer aur
//  Receptionist dono User ko extend karte hain (models/User.h).
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_ENUMS_USERROLE_H
#define CAR_RENTAL_SYSTEM_LLD_ENUMS_USERROLE_H

namespace car_rental_lld {

enum class UserRole { CUSTOMER, RECEPTIONIST };

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_ENUMS_USERROLE_H
