// ============================================================================
//  models/Requests.h  —  Do tarah ki request (Requirement #13, #14)
// ----------------------------------------------------------------------------
//  Elevator ko bulane ke DO bilkul alag tareeke hain — aur ye farak samajhna
//  poore design ki jad hai:
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  EXTERNAL (bahar se — floor panel)      | INTERNAL (andar se — inside     │
//  │                                          |            panel)              │
//  │  ─────────────────────────────────────── | ────────────────────────────── │
//  │  "Koi bhi elevator bhejo, mujhe UPAR    | "MAIN Elevator#2 me hu, mujhe  │
//  │   jaana hai"                             |  7th floor le chalo"           │
//  │                                          |                                │
//  │  Chahiye: floor + DIRECTION              | Chahiye: elevatorId + floor    │
//  │  Elevator kaunsa? -> SYSTEM decide karta | Elevator kaunsa? -> pata hai!  │
//  │                     (smart dispatch #22) |    (tum usi me baithe ho)      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Isi liye DO alag struct hain, ek nahi:
//
//     ExternalRequest me `elevatorId` NAHI hai — kyunki bahar khade bande ko
//     pata hi nahi ki kaunsa elevator aayega! Wo bas "koi bhi bhejo" bolta hai.
//     Agar zabardasti ek hi struct banate (dono fields ke saath), to ek field
//     hamesha khaali/bekaar padi rehti — aur padhne wale ko confusion hoti ki
//     "isme elevatorId bharu ya nahi?"
//
//     InternalRequest me `direction` NAHI hai — kyunki tum andar baithe ho aur
//     bas floor number daba rahe ho. UP ya DOWN? Wo elevator KHUD samajh lega
//     (tumhari current floor se compare karke — ElevatorCar::addStop dekho).
//
//  📌 SABAK: har struct me sirf WAHI data rakho jo us case me ASAL ME hota hai.
//     "Ek bada struct bana ke sab kuch usme daal do" aasan lagta hai, par usse
//     har jagah khaali fields aur `if (field khaali hai to...)` wale checks
//     paida ho jaate hain. Do chhote struct >> ek bada confusing struct.
//
//  💡 `struct` kyun, `class` kyun nahi? Kyunki inme sirf DATA hai, koi logic
//     nahi (behavior nahi). C++ me convention hai: sirf-data wali cheez =
//     `struct` (sab public), logic wali = `class`. Ye padhne wale ko turant
//     signal deta hai ki "ye bas ek data ka dabba hai".
// ============================================================================
#ifndef ELEVATOR_SYSTEM_LLD_MODELS_REQUESTS_H
#define ELEVATOR_SYSTEM_LLD_MODELS_REQUESTS_H

#include <bits/stdc++.h>

#include "../enums/Direction.h"

using namespace std;

namespace elevator_lld {

// ---- BAHAR se request (Requirement #13, #15) -------------------------------
// Floor panel ka UP/DOWN button dabane pe ye banti hai.
// Scheduler isi ke `direction` ko dekh ke best elevator chunta hai (Req #22).
struct ExternalRequest {
    int floor;           // kis floor se bulaya
    Direction direction; // us bande ko kis taraf jaana hai (UP/DOWN)
};

// ---- ANDAR se request (Requirement #14) ------------------------------------
// Inside panel ka floor button dabane pe ye banti hai.
struct InternalRequest {
    int elevatorId;      // kis elevator me baithe ho (pata hai!)
    int destinationFloor; // kahan jaana hai
};

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_MODELS_REQUESTS_H
