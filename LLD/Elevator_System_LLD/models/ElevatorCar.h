// ============================================================================
//  models/ElevatorCar.h  —  Ek elevator car (poore system ka dil)
// ----------------------------------------------------------------------------
//  Ek elevator car ke paas ye cheezein hain:
//     1. HAALAT   -> kaunsi floor pe hai, kis direction me ja raha hai
//     2. STOPS    -> kahan-kahan rukna hai (upStops_ / downStops_)
//     3. DARWAZA  -> Door           (Requirement #11, #12)
//     4. CAPACITY -> log + weight   (Requirement #19, #20)
//     5. DISPLAYS -> andar aur bahar (Requirement #16, #17)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DO SET kyun? (upStops_ aur downStops_)                               │
//  │                                                                          │
//  │  Asli elevator ka behavior socho: tum 5th floor pe ho aur UPAR ja rahe   │
//  │  ho. Raaste me 7 aur 9 pe rukna hai. Ab koi 2nd floor se bulata hai —    │
//  │  kya tum turant neeche mud jaoge? NAHI! Pehle 7 aur 9 nipta ke, PHIR     │
//  │  neeche aaoge. Yahi asli elevator karta hai.                             │
//  │                                                                          │
//  │  Isi liye stops DO alag sets me hain:                                    │
//  │     upStops_   -> `set<int>`                (chhota se bada: 7, 9)       │
//  │     downStops_ -> `set<int, greater<int>>`  (bada se chhota: 9, 7)       │
//  │                                                                          │
//  │  ⭐ Dono ka ORDER ULTA hai — aur ye chalaki hai:                         │
//  │     UP   jaate waqt agla stop = SABSE CHHOTA bacha hua -> `*begin()`     │
//  │     DOWN jaate waqt agla stop = SABSE BADA  bacha hua  -> `*begin()`     │
//  │  Comparator ulta rakhne se DONO me `*begin()` hi "agla stop" deta hai —  │
//  │  alag logic likhne ki zaroorat hi nahi. Safai wali trick hai.            │
//  │                                                                          │
//  │  Aur `set` isliye (vector nahi): duplicate stops apne aap hat jaate hain │
//  │  (ek hi button 5 baar dabao -> stop ek hi banega), aur hamesha sorted.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 SAFETY INVARIANT (Requirement #11) — kabhi na tootne wala rule:
//        door == OPEN   =>   direction == IDLE
//     Yaani CHALTI elevator ka darwaza kabhi khula nahi ho sakta. Ye DO jagah
//     lagu hota hai:
//       - `Door::open()`  -> khulne se pehle poochta hai "elevator ruka hai?"
//       - `step()`        -> chalne se PEHLE darwaza zabardasti band karta hai
//     Ek hi kaafi hota, par dono milke isko todna namumkin bana dete hain.
//     Ise "defense in depth" kehte hain.
// ============================================================================
#ifndef ELEVATOR_SYSTEM_LLD_MODELS_ELEVATORCAR_H
#define ELEVATOR_SYSTEM_LLD_MODELS_ELEVATORCAR_H

#include <bits/stdc++.h>

#include "../config/SystemLimits.h"
#include "../enums/Direction.h"
#include "Display.h"
#include "Door.h"

using namespace std;

namespace elevator_lld {

class ElevatorCar {
public:
    ElevatorCar(int id, int minFloor, int maxFloor)
        : id_(id),
          minFloor_(minFloor),
          maxFloor_(maxFloor),
          currentFloor_(minFloor),
          direction_(Direction::IDLE),
          peopleCount_(0),
          currentWeightKg_(0),
          insideDisplay_(DisplayType::INSIDE),    // #17 — capacity bhi dikhata hai
          outsideDisplay_(DisplayType::OUTSIDE) { // #16 — sirf floor + direction
        refreshDisplays();
    }

    // ---- Basic getters ------------------------------------------------------
    int getId() const { return id_; }
    int getCurrentFloor() const { return currentFloor_; }
    Direction getDirection() const { return direction_; }

    // Idle = na koi direction, na koi pending stop. Yaani bilkul free.
    // ⭐ Dono check zaroori hain: direction IDLE ho par stops pending hon, to wo
    // asal me idle nahi — agle tick me chal padega.
    bool isIdle() const {
        return direction_ == Direction::IDLE && upStops_.empty() && downStops_.empty();
    }

    // ========================================================================
    //  DOOR (Requirement #11, #12)
    // ========================================================================

    // Darwaza kholne ki koshish. Sirf tab khulega jab elevator RUKA ho.
    //
    // ⭐ Notice: hum Door ko elevator ka pointer nahi dete — bas ek simple sach
    // bhejte hain ("ruka hai ya nahi"). Isse Door bilkul aazaad rehta hai aur
    // akele test ho sakta hai. (Poora explanation Door.h me hai.)
    bool openDoor() { return door_.open(direction_ == Direction::IDLE); }

    void closeDoor() { door_.close(); }
    bool isDoorOpen() const { return door_.isOpen(); }
    DoorState getDoorState() const { return door_.getState(); }

    // ========================================================================
    //  CAPACITY (Requirement #19, #20)
    // ========================================================================

    int getPeopleCount() const { return peopleCount_; }
    int getCurrentWeight() const { return currentWeightKg_; }

    bool isFull() const { return peopleCount_ >= MAX_PEOPLE || currentWeightKg_ >= MAX_WEIGHT_KG; }

    // ---- Passenger chadhne ki koshish --------------------------------------
    //
    // ⭐ DO-TARFA limit (Requirement #19) — dono me se jo PEHLE bhar jaaye:
    //       8 log   YA   680 kg
    //
    // Sirf ek check karna kaafi NAHI. Socho:
    //   - 8 bachche (30 kg each) = 240 kg -> weight se jagah hai, par log 8 -> FULL
    //   - 5 bade log (140 kg each) = 700 kg -> log sirf 5, par weight paar -> FULL
    // Isi liye dono conditions ALAG-ALAG check hoti hain.
    //
    // Return: chadh gaya (true) ya reject hua (false) — caller ko pata chalna
    // chahiye. `void` hota to overload chup-chaap ignore ho jaata.
    bool boardPassenger(int weightKg = DEFAULT_PASSENGER_WEIGHT_KG) {
        // Darwaza band hai to koi chadh hi nahi sakta.
        // (Requirement me alag se likha nahi hai — ye physics hai 😄. Par isse
        //  door aur capacity ke rules aapas me consistent ho jaate hain.)
        if (!door_.isOpen()) {
            cout << "  [Elevator#" << id_ << "] ❌ Darwaza band hai — koi chadh nahi sakta\n";
            return false;
        }

        // Check 1 — LOGON ki ginti
        if (peopleCount_ + 1 > MAX_PEOPLE) {
            cout << "  [Elevator#" << id_ << "] ❌ OVERLOAD! " << MAX_PEOPLE << " log ki limit full hai (abhi "
                 << peopleCount_ << " hain)\n";
            return false;
        }

        // Check 2 — WEIGHT
        if (currentWeightKg_ + weightKg > MAX_WEIGHT_KG) {
            cout << "  [Elevator#" << id_ << "] ❌ OVERLOAD! Weight limit " << MAX_WEIGHT_KG << " kg (abhi "
                 << currentWeightKg_ << " kg, aur " << weightKg << " kg nahi aa sakta)\n";
            return false;
        }

        peopleCount_++;
        currentWeightKg_ += weightKg;
        cout << "  [Elevator#" << id_ << "] ✅ Passenger chadha (" << weightKg << " kg) -> ab " << peopleCount_
             << " log, " << currentWeightKg_ << " kg\n";
        refreshDisplays();
        return true;
    }

    // ---- Passenger utarna --------------------------------------------------
    bool exitPassenger(int weightKg = DEFAULT_PASSENGER_WEIGHT_KG) {
        if (peopleCount_ <= 0) {
            cout << "  [Elevator#" << id_ << "] Koi hai hi nahi utarne ke liye\n";
            return false;
        }
        peopleCount_--;

        // ⭐ `max(0, ...)` — weight kabhi NEGATIVE na ho.
        // Kyun zaroori? Chadhte waqt asli weight (100 kg) diya ho aur utarte
        // waqt default (85 kg) — to hisaab bigad sakta hai. Ye guard usko safe
        // rakhta hai. (Asli system me har passenger ka weight track hota hai;
        //  yahan jaan-boojh ke simple rakha hai.)
        currentWeightKg_ = max(0, currentWeightKg_ - weightKg);

        cout << "  [Elevator#" << id_ << "] Passenger utra -> ab " << peopleCount_ << " log, " << currentWeightKg_
             << " kg\n";
        refreshDisplays();
        return true;
    }

    // ========================================================================
    //  STOPS aur MOVEMENT
    // ========================================================================

    // Naya stop add karo (bahar se call, ya andar se floor button).
    void addStop(int floor) {
        validateFloor(floor);
        if (floor > currentFloor_) {
            upStops_.insert(floor);
        } else if (floor < currentFloor_) {
            downStops_.insert(floor);
        }
        // floor == currentFloor_ -> yahin to khade hain! Stop add karne ki
        // zaroorat hi nahi (add karte to elevator khud ke floor pe "rukne" ki
        // koshish karta aur logic ganda ho jaata).
    }

    // ---- Ek tick aage badho (poore system ka engine) ------------------------
    void step() {
        // Kuch karna hi nahi bacha -> IDLE ho ke ruk jao.
        if (upStops_.empty() && downStops_.empty()) {
            direction_ = Direction::IDLE;
            refreshDisplays();
            return;
        }

        // ⭐⭐ SAFETY RULE (Requirement #11) — chalne se PEHLE darwaza band karo.
        //
        // Ye invariant ki DOOSRI layer hai. `Door::open()` khulne se rokta hai
        // jab elevator chal raha ho — par ULTA case bhi hota hai: darwaza khula
        // tha (elevator ruka hua tha), aur AB elevator ko chalna hai! Us waqt
        // darwaza zabardasti band karna PADEGA.
        //
        // Dono milke pakka kar dete hain: "chalti elevator = band darwaza",
        // hamesha, bina kisi exception ke.
        if (door_.forceClose()) {
            cout << "  [Elevator#" << id_ << "] Chalne se pehle darwaza auto-band\n";
        }

        // Ab tak IDLE the? To pehle decide karo kis taraf jaana hai.
        if (direction_ == Direction::IDLE) {
            direction_ = pickInitialDirection();
        }

        if (direction_ == Direction::UP) {
            processUpDirection();
        } else {
            processDownDirection();
        }

        refreshDisplays();
    }

    // ========================================================================
    //  DISPLAYS (Requirement #16, #17)
    // ========================================================================

    void showDisplays() const {
        string label = "Elevator#" + to_string(id_);
        outsideDisplay_.show(label); // bahar wala — floor + direction
        insideDisplay_.show(label);  // andar wala  — + capacity (#17)
    }

    // ---- Ek line me poora status (debugging ke liye) -----------------------
    void printState() const {
        cout << "Elevator#" << id_ << " | Floor: " << currentFloor_ << " | Direction: " << directionToString(direction_)
             << " | Door: " << doorStateToString(door_.getState()) << " | " << peopleCount_ << " log/"
             << currentWeightKg_ << "kg\n";
    }

private:
    int id_;
    int minFloor_;
    int maxFloor_;
    int currentFloor_;
    Direction direction_;

    Door door_; // #11, #12

    int peopleCount_;     // #19, #20
    int currentWeightKg_; // #19, #20

    Display insideDisplay_;  // #17
    Display outsideDisplay_; // #16

    // ---- Stops — order ULTA hai, jaan-boojh ke (upar wala note padho) ----
    set<int> upStops_;                 // chhota->bada : *begin() = agla upar wala stop
    set<int, greater<int>> downStops_; // bada->chhota : *begin() = agla neeche wala stop

    // Har haalat badalne pe displays ko naya data bhejo.
    // (Display khud kuch nahi maangta — usko BATAYA jaata hai. Display.h dekho.)
    void refreshDisplays() {
        insideDisplay_.update(currentFloor_, direction_, peopleCount_, currentWeightKg_);
        outsideDisplay_.update(currentFloor_, direction_, peopleCount_, currentWeightKg_);
    }

    void validateFloor(int floor) const {
        if (floor < minFloor_ || floor > maxFloor_) {
            throw runtime_error("Invalid floor request: " + to_string(floor));
        }
    }

    // ---- Ab tak ruke the — kis taraf jaana chahiye? ------------------------
    Direction pickInitialDirection() const {
        // Dono taraf stops hain? To jo NAZDEEK ho, udhar jao.
        if (!upStops_.empty() && !downStops_.empty()) {
            const int nearestUp = *upStops_.begin();     // sabse chhota upar wala
            const int nearestDown = *downStops_.begin(); // sabse bada neeche wala
            const int upDistance = abs(nearestUp - currentFloor_);
            const int downDistance = abs(nearestDown - currentFloor_);
            // Tie pe UP chunte hain (`<=`). Koi khaas wajah nahi — bas ek
            // consistent faisla chahiye tha taaki behavior deterministic rahe
            // (yaani same input pe hamesha same output).
            return (upDistance <= downDistance) ? Direction::UP : Direction::DOWN;
        }
        if (!upStops_.empty()) {
            return Direction::UP;
        }
        return Direction::DOWN;
    }

    // ---- UP direction me ek kadam -----------------------------------------
    void processUpDirection() {
        // Upar kuch nahi bacha? To ya neeche mudo, ya ruk jao.
        if (upStops_.empty()) {
            direction_ = downStops_.empty() ? Direction::IDLE : Direction::DOWN;
            return;
        }

        ++currentFloor_; // ek floor upar

        // Kya is floor pe rukna tha?
        auto it = upStops_.find(currentFloor_);
        if (it != upStops_.end()) {
            upStops_.erase(it);
            cout << "Elevator#" << id_ << " stopped at floor " << currentFloor_ << "\n";
        }

        // ⭐ Ab aage kya? Ye teen line hi poora "asli elevator jaisa" behavior deti hain:
        //     upar aur stops hain     -> UP hi rehna (kuch mat karo)
        //     upar khatam, neeche hai -> ab DOWN mudo
        //     dono khatam             -> IDLE (aur ab darwaza khul sakega,
        //                                kyunki elevator ruk gaya)
        if (upStops_.empty() && !downStops_.empty()) {
            direction_ = Direction::DOWN;
        } else if (upStops_.empty()) {
            direction_ = Direction::IDLE;
        }
    }

    // ---- DOWN direction me ek kadam (upar wale ka bilkul mirror) -----------
    void processDownDirection() {
        if (downStops_.empty()) {
            direction_ = upStops_.empty() ? Direction::IDLE : Direction::UP;
            return;
        }

        --currentFloor_; // ek floor neeche

        auto it = downStops_.find(currentFloor_);
        if (it != downStops_.end()) {
            downStops_.erase(it);
            cout << "Elevator#" << id_ << " stopped at floor " << currentFloor_ << "\n";
        }

        if (downStops_.empty() && !upStops_.empty()) {
            direction_ = Direction::UP;
        } else if (downStops_.empty()) {
            direction_ = Direction::IDLE;
        }
    }
};

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_MODELS_ELEVATORCAR_H
