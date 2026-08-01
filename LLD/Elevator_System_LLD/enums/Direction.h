// ============================================================================
//  enums/Direction.h  —  Elevator kis taraf ja raha hai (Requirement #9)
// ----------------------------------------------------------------------------
//  "Elevators can move up, move down, or stay idle (not moving)."
//  Bas teen haalat — UP, DOWN, IDLE. Aur kuch nahi.
//
//  ⭐ IDLE ek asli "direction" hai kya? Technically nahi — par ise yahan rakhna
//     sahi hai, aur ye ek accha design sabak hai:
//
//     Agar IDLE alag rakhte (jaise `bool isMoving` + `Direction dir`), to DO
//     variables sambhalne padte, aur wo aapas me JHOOTH bol sakte the:
//         isMoving = false, dir = UP    <- ye kya matlab? ruka hua hai par
//                                          upar ja raha hai?? 🤷
//     Ab ek hi variable hai, to aisi ulti-pulti haalat ban hi nahi sakti.
//
//     📌 Sabak: "kuch nahi ho raha" ko bhi ek ASLI value banao (IDLE), do
//        variables se mat sambhalo. (L33 me Board ka `emptyCell_` bhi yahi
//        idea tha — Null Object pattern.)
//
//  ⭐ Ye enum poore system me sabse zyada use hota hai:
//     - ElevatorCar   -> abhi kis taraf ja raha hai
//     - FloorPanel    -> passenger ko kis taraf jaana hai (UP/DOWN button)
//     - Scheduler     -> DONO ko compare karke best elevator chunta hai (#22)
//     - Display       -> screen pe direction dikhata hai (#16)
//
//  `enum class` (scoped) hai, plain `enum` nahi — `Direction::UP` likhna padta
//  hai (naam bahar leak nahi hota), aur `int` me chupke se convert nahi hota.
//  Type safety free me. (DoorState.h me bhi yahi kiya hai — consistent rehna
//  acchi aadat hai.)
// ============================================================================
#ifndef ELEVATOR_SYSTEM_LLD_ENUMS_DIRECTION_H
#define ELEVATOR_SYSTEM_LLD_ENUMS_DIRECTION_H

#include <bits/stdc++.h>

using namespace std;

namespace elevator_lld {

enum class Direction { UP, DOWN, IDLE };

// Enum ko padhne layak string me badalta hai — printing/display ke liye.
// `inline` isliye taaki ye header kai .cpp me include ho to linker "duplicate
// symbol" ka error na de.
inline string directionToString(Direction direction) {
    if (direction == Direction::UP) {
        return "UP";
    }
    if (direction == Direction::DOWN) {
        return "DOWN";
    }
    return "IDLE";
}

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_ENUMS_DIRECTION_H
