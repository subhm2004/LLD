// ============================================================================
//  models/Slot.h  —  Machine ka ek KHAANA (Requirement R1, R5)
// ----------------------------------------------------------------------------
//  "R1: The vending machine has different products in various slots."
//  "R5: Users can pick a product to buy by entering the slot number."
//
//  Asli vending machine dekho — uspe ek grid hota hai, aur har khaane pe ek
//  number likha hota hai (101, 102, 103...). Tum wo number dabate ho, aur us
//  khaane ka saamaan gir jaata hai. Wahi khaana = SLOT.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SLOT aur ITEM ek cheez NAHI hain — ye farak samajhna zaroori hai     │
//  │                                                                          │
//  │     ITEM -> "Coke Can, ₹40, type COKE"   (product KYA hai)               │
//  │     SLOT -> "khaana 101, isme Coke hai, 3 bache hain"  (product KAHAN    │
//  │                                                          hai, kitne hain) │
//  │                                                                          │
//  │  Do alag slots me SAME item ho sakta hai! Jaise slot 101 aur 105 dono me │
//  │  Coke — ye asli machines me hota hai (zyada demand wali cheez do jagah). │
//  │  Isi liye quantity SLOT me hai, Item me nahi.                            │
//  │                                                                          │
//  │  📌 Purana code items ko `ItemType` (enum) se pehchanta tha — matlab ek  │
//  │     type ka saamaan sirf EK hi jagah ho sakta tha. Slots aane se ye      │
//  │     bandish khatam. Ye R1 ka asli matlab hai.                            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  💡 Item ko slot ke andar VALUE me rakha hai (`Item item_`), pointer me nahi.
//     Kyun? Kyunki Item ek chhota "value object" hai (naam, daam, type — bas).
//     Pointer rakhne se `new`/`delete` ki jhanjhat aati, aur koi fayda nahi.
//     📌 Rule: chhoti value-type cheezon ko value me rakho. Pointer sirf tab
//        jab polymorphism chahiye ho ya object bahut bada ho.
// ============================================================================
#ifndef VENDING_MACHINE_LLD_MODELS_SLOT_H
#define VENDING_MACHINE_LLD_MODELS_SLOT_H

#include <stdexcept>
#include <string>

#include "Item.h"

using namespace std;

namespace vending_machine_lld {

class Slot {
public:
    // Khaali slot (koi saamaan nahi bhara). Machine me shuru me aise hi hote hain.
    Slot() : slotNumber_(0), quantity_(0), occupied_(false) {}

    Slot(int slotNumber, const Item &item, int quantity)
        : slotNumber_(slotNumber), item_(item), quantity_(quantity), occupied_(true) {}

    int getSlotNumber() const { return slotNumber_; }
    const Item &getItem() const { return item_; }
    int getQuantity() const { return quantity_; }

    // ⭐ `occupied_` aur `quantity_ > 0` ek cheez NAHI hain — dhyaan se:
    //     occupied_ = false          -> slot me kuch bhara hi nahi (khaali khaana)
    //     occupied_ = true, qty = 0  -> Coke bhara tha, par sab bik gaya
    //
    // Farak kyun matter karta hai? Kyunki dusre case me machine ko PATA hai ki
    // yahan Coke aana chahiye (daam bhi pata hai) — bas stock khatam hai. Wo
    // user ko "Coke out of stock" bol sakti hai. Pehle case me to slot ka koi
    // matlab hi nahi — "invalid slot" bolegi.
    bool isOccupied() const { return occupied_; }
    bool hasStock() const { return occupied_ && quantity_ > 0; }

    // ---- Ek saamaan nikla (bik gaya) ---------------------------------------
    void reduceQuantity() {
        if (!hasStock()) {
            throw runtime_error("Slot " + to_string(slotNumber_) + " me stock nahi hai");
        }
        --quantity_;
    }

    // ---- Admin ne aur stock bhara (R4) -------------------------------------
    void addQuantity(int extra) {
        if (extra <= 0) {
            throw runtime_error("Quantity positive honi chahiye (mili: " + to_string(extra) + ")");
        }
        quantity_ += extra;
    }

    // ---- Admin ne poora slot khaali kar diya (R4) --------------------------
    //
    // Sirf quantity 0 nahi kar rahe — `occupied_` bhi false. Yaani "yahan ab
    // koi product hai hi nahi", "product hai par khatam hai" nahi. (Upar wala
    // note padho — ye dono alag cheezein hain.)
    void clear() {
        item_ = Item();
        quantity_ = 0;
        occupied_ = false;
    }

private:
    int slotNumber_; // 101, 102, ... — user yahi number dabata hai (R5)
    Item item_;      // is khaane me kaunsa product hai
    int quantity_;   // kitne bache hain
    bool occupied_;  // slot me kuch bhara bhi hai ya nahi (upar note padho)
};

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_MODELS_SLOT_H
