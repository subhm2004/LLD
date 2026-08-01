// ============================================================================
//  enums/DoorState.h  —  Darwaza khula hai ya band (Requirement #11)
// ----------------------------------------------------------------------------
//  Sirf do haalat hain: OPEN ya CLOSED. Bas.
//
//  ⭐ `bool isOpen` se kaam nahi chal jaata? Chal jaata — par enum behtar hai:
//
//    1. PADHNE ME saaf: `door.getState() == DoorState::OPEN` vs `door.isOpen()`
//       — dono theek, par jab code me `setState(true)` likha ho to padhne wale
//       ko sochna padta hai "true ka matlab open ya closed?". Enum me confusion
//       hai hi nahi.
//
//    2. AAGE BADHNE KI JAGAH: kal ko real system me aur states chahiye —
//       OPENING, CLOSING, BLOCKED (koi cheez darwaze me phansi hai), FAULTY.
//       `bool` me ye ho hi nahi sakta; enum me bas ek entry add karo.
//       Asli elevators me ye states hote bhi hain.
//
//  ⭐ `enum class` (scoped) use kiya hai, plain `enum` nahi:
//     - `DoorState::OPEN` likhna padta hai — naam bahar leak nahi hota
//       (warna `OPEN` naam poore namespace me ghus jaata, aur kisi aur cheez
//        se takra sakta tha)
//     - `int` me chupke se convert nahi hota -> galti se `if (state == 1)`
//       likhna compile hi nahi hoga. Type safety free me mil gayi.
// ============================================================================
#ifndef ELEVATOR_SYSTEM_LLD_ENUMS_DOORSTATE_H
#define ELEVATOR_SYSTEM_LLD_ENUMS_DOORSTATE_H

#include <string>

using namespace std;

namespace elevator_lld {

enum class DoorState { OPEN, CLOSED };

// Printing/debugging ke liye — enum ko padhne layak string me badalta hai.
// (`Direction.h` me bhi bilkul aisa hi `directionToString` hai — ek jaisa
//  pattern poore codebase me rakhna acchi aadat hai, padhne wala turant
//  samajh jaata hai.)
//
// `inline` isliye taaki ye header agar kai .cpp me include ho to linker
// "duplicate symbol" ka error na de.
inline string doorStateToString(DoorState state) {
    return (state == DoorState::OPEN) ? "OPEN" : "CLOSED";
}

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_ENUMS_DOORSTATE_H
