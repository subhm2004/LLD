// ============================================================================
//  models/Door.h  —  Elevator ka darwaza (Requirement #11, #12)
// ----------------------------------------------------------------------------
//  Ek chhoti si class jiska ek hi kaam hai: darwaze ki haalat sambhalna, aur
//  ek SAFETY RULE lagu karna.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SABSE ZAROORI NIYAM (Requirement #11):                               │
//  │                                                                          │
//  │     "Darwaza SIRF tab khul sakta hai jab elevator CHAL na raha ho."      │
//  │                                                                          │
//  │  Ye sirf ek feature nahi — ye SAFETY ka mamla hai. Chalti elevator ka    │
//  │  darwaza khul gaya to koi gir sakta hai. Isi liye ye rule code me        │
//  │  DO jagah lagu hota hai (defense in depth):                             │
//  │                                                                          │
//  │     1. YAHAN (`Door::open`)  -> khulne se pehle poochta hai "elevator    │
//  │                                  ruka hua hai kya?" Nahi -> mana kar do. │
//  │     2. `ElevatorCar::step()` -> chalne se PEHLE darwaza zabardasti band  │
//  │                                  kar deta hai (auto-close).              │
//  │                                                                          │
//  │  Ek hi kaafi hota, par dono hone se rule kabhi toot hi nahi sakta.       │
//  │  Aise "kabhi na tootne wale" rules ko INVARIANT kehte hain.              │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 DESIGN CHOICE — Door ko elevator ke baare me kuch NAHI pata:
//     Dhyaan do ki `open()` ek `bool elevatorIsIdle` parameter leta hai — Door
//     ke paas `ElevatorCar*` ka pointer NAHI hai. Ye jaan-boojh ke hai:
//
//       - Agar Door ke paas machine ka pointer hota, to dono ek doosre se
//         BANDH jaate (circular dependency), aur Door ko akele test karna
//         mushkil ho jaata.
//       - Ab Door ek "bewakoof" chhota component hai: usse bas sach batao
//         ("elevator ruka hai ya nahi"), aur wo apna kaam kar dega.
//
//     Ise DEPENDENCY INVERSION ki chhoti si misaal samjho — neeche wala
//     component upar wale ko nahi jaanta, bas ek simple fact maangta hai.
//     Isi liye Door ko test karna bahut aasan hai: `d.open(true)` / `d.open(false)`.
// ============================================================================
#ifndef ELEVATOR_SYSTEM_LLD_MODELS_DOOR_H
#define ELEVATOR_SYSTEM_LLD_MODELS_DOOR_H

#include <iostream>

#include "../enums/DoorState.h"

using namespace std;

namespace elevator_lld {

class Door {
public:
    // Darwaza hamesha BAND se shuru hota hai — ye safe default hai.
    // (Socho agar khula se shuru hota aur koi `close()` bulana bhool jaata...
    //  Hamesha safe side se shuru karo.)
    Door() : state_(DoorState::CLOSED) {}

    DoorState getState() const { return state_; }
    bool isOpen() const { return state_ == DoorState::OPEN; }
    bool isClosed() const { return state_ == DoorState::CLOSED; }

    // ---- Darwaza kholo (Requirement #11) -----------------------------------
    //
    // `elevatorIsIdle` -> caller batata hai ki elevator ruka hua hai ya nahi.
    // Return: khul gaya (true) ya mana kar diya (false).
    //
    // ⭐ Return value `bool` hai, `void` nahi — taaki caller ko PATA chale ki
    // uska kaam hua ya nahi. `void` hota to darwaza chup-chaap na khulta aur
    // caller khush rehta ki "ho gaya". Aisi chup-chaap failure hi bugs ki jad
    // hoti hai.
    bool open(bool elevatorIsIdle) {
        // ⭐⭐ YE HAI WO SAFETY RULE — poore system ki sabse zaroori line
        if (!elevatorIsIdle) {
            cout << "  [DOOR] ❌ Elevator chal raha hai — darwaza nahi khul sakta!\n";
            return false;
        }

        // Pehle se khula hai? To kuch karne ki zaroorat nahi.
        // (Ye "idempotent" behavior hai — ek hi cheez do baar bolne se kuch
        //  nahi bigadta. Real button pe log baar-baar dabate hain, to ye
        //  behavior zaroori hai.)
        if (state_ == DoorState::OPEN) {
            cout << "  [DOOR] Darwaza pehle se khula hai\n";
            return true;
        }

        state_ = DoorState::OPEN;
        cout << "  [DOOR] ✅ Darwaza khul gaya\n";
        return true;
    }

    // ---- Darwaza band karo -------------------------------------------------
    //
    // Notice: band karne me KOI shart nahi hai. Kyun? Kyunki darwaza band karna
    // hamesha SAFE hai — chahe elevator chale ya na chale. Sirf KHOLNA khatarnak
    // hai. Isi liye `open()` me guard hai aur `close()` me nahi.
    //
    // 📌 Sabak: guard wahin lagao jahan KHATRA hai. Har function pe check
    //    thonk dena code ko ganda karta hai, surakshit nahi.
    void close() {
        if (state_ == DoorState::CLOSED) {
            cout << "  [DOOR] Darwaza pehle se band hai\n";
            return;
        }
        state_ = DoorState::CLOSED;
        cout << "  [DOOR] Darwaza band ho gaya\n";
    }

    // ---- Zabardasti band karo (elevator chalne se pehle) -------------------
    //
    // `close()` se farak sirf itna: ye CHUP-CHAAP band karta hai (koi message
    // nahi), aur batata hai ki asal me kuch band karna pada tha ya nahi.
    //
    // Ye `ElevatorCar::step()` har baar chalne se pehle bulata hai. Agar har
    // baar "Darwaza band ho gaya" print hota, to output kachre se bhar jaata —
    // kyunki 99% baar darwaza pehle se band hi hota hai.
    //
    // ⭐ Yahi wo doosri layer hai jo safety rule ko pakka karti hai: chahe
    // koi bhi galti kare, elevator CHALNE SE PEHLE darwaza band ho hi jaayega.
    bool forceClose() {
        if (state_ == DoorState::CLOSED) {
            return false; // pehle se band tha — kuch karna nahi pada
        }
        state_ = DoorState::CLOSED;
        return true; // haan, band karna pada
    }

private:
    DoorState state_;
};

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_MODELS_DOOR_H
