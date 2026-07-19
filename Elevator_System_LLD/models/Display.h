// ============================================================================
//  models/Display.h  —  Elevator ke display (Requirement #16, #17, #18)
// ----------------------------------------------------------------------------
//  Requirements teen tarah ke display maangte hain:
//
//     #16  Elevator ke ANDAR aur BAHAR  -> current floor + direction
//     #17  ANDAR wala EXTRA dikhata hai -> capacity (kitne log / kitna weight)
//     #18  Har FLOOR pe                 -> elevators ka status
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Teen display, par TEEN class nahi — ek class + ek "type" flag        │
//  │                                                                          │
//  │  Socho: inside aur outside display me farak kya hai? Sirf itna ki inside │
//  │  wala capacity bhi dikhata hai. Baaki dono bilkul same — floor aur       │
//  │  direction. To teen alag classes banana bekaar hai (copy-paste code).    │
//  │                                                                          │
//  │  Isi liye ek `Display` class hai jo `DisplayType` se jaanti hai ki usko  │
//  │  capacity dikhani hai ya nahi.                                           │
//  │                                                                          │
//  │  ⚠ Par dhyaan: agar aage chal ke inside/outside ka behavior BAHUT alag   │
//  │  ho jaye, to tab alag classes banana behtar hoga (ya Strategy pattern).  │
//  │  Abhi farak chhota hai (ek line ka), to ek class hi sahi hai.            │
//  │  📌 "Do cheezein 90% same hain" -> ek class. "40% same" -> alag classes. │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Display ek "DUMB" component hai — wo khud kuch nahi jaanta:
//     - Elevator ka pointer NAHI rakhta
//     - Kisi se data khud nahi maangta
//     Bas `update(...)` se use data BATAYA jaata hai, aur `show()` se wo print
//     kar deta hai. (Door ki tarah — dono jaan-boojh ke bewakoof rakhe hain.)
//
//     Fayda: Display ko akele test kar sakte ho, aur elevator badal do to
//     display ka code chhuna hi nahi padta.
// ============================================================================
#ifndef ELEVATOR_SYSTEM_LLD_MODELS_DISPLAY_H
#define ELEVATOR_SYSTEM_LLD_MODELS_DISPLAY_H

#include <iostream>
#include <string>

#include "../enums/Direction.h"

using namespace std;

namespace elevator_lld {

// Kis tarah ka display hai — isse tay hota hai ki kya-kya dikhega.
enum class DisplayType {
    INSIDE,  // elevator ke andar  -> floor + direction + CAPACITY (#17)
    OUTSIDE, // elevator ke bahar  -> floor + direction (#16)
    FLOOR    // floor pe laga hua  -> floor + direction (#18)
};

class Display {
public:
    explicit Display(DisplayType type) : type_(type), floor_(0), direction_(Direction::IDLE), people_(0), weight_(0) {}

    // ---- Display pe naya data bhejo ----------------------------------------
    //
    // Elevator har tick ke baad ye bulata hai. Display khud kuch nahi maangta —
    // usko BATAYA jaata hai. (Ise "push" model kehte hain; "pull" model me
    // display khud elevator se poochta, par tab usko elevator ka pointer
    // rakhna padta aur dono bandh jaate.)
    void update(int floor, Direction direction, int people, int weight) {
        floor_ = floor;
        direction_ = direction;
        people_ = people;
        weight_ = weight;
    }

    // ---- Display pe dikhao --------------------------------------------------
    //
    // `label` -> kiska display hai ("Elevator#1", "Floor 3") — taaki output me
    // pata chale ye kahan ka display hai.
    void show(const string &label) const {
        cout << "  [" << displayTypeName() << " DISPLAY | " << label << "] "
             << "Floor: " << floor_ << " | Direction: " << directionToString(direction_);

        // ⭐ Requirement #17 — capacity SIRF andar wale display pe.
        // Bahar khade bande ko ye jaanne ki zaroorat nahi ki andar kitne log
        // hain... par andar wale ko hai (taaki pata chale aur log aa sakte hain
        // ya nahi).
        if (type_ == DisplayType::INSIDE) {
            cout << " | Capacity: " << people_ << " log, " << weight_ << " kg";
        }
        cout << "\n";
    }

    DisplayType getType() const { return type_; }

private:
    DisplayType type_;
    int floor_;
    Direction direction_;
    int people_; // sirf INSIDE display use karta hai
    int weight_; // sirf INSIDE display use karta hai

    // Display ke type ka naam — printing ke liye.
    string displayTypeName() const {
        if (type_ == DisplayType::INSIDE) return "INSIDE";
        if (type_ == DisplayType::OUTSIDE) return "OUTSIDE";
        return "FLOOR";
    }
};

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_MODELS_DISPLAY_H
