// ============================================================================
//  services/Inventory.h  —  Machine ka stock (Requirement R1, R4, R5)
// ----------------------------------------------------------------------------
//  Inventory ka ek hi kaam hai: SLOTS sambhalna. Kaunse slot me kya hai, kitna
//  hai, aur admin usme kya badal sakta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ PURANA CODE `ItemType` (enum) se keyed tha — ab SLOT NUMBER se       │
//  │                                                                          │
//  │  Pehle aisa tha:                                                         │
//  │      unordered_map<ItemType, Item> items_;                               │
//  │      unordered_map<ItemType, int>  stock_;                               │
//  │                                                                          │
//  │  Do problem thi:                                                         │
//  │    1. R1/R5 kehte hain "products in various SLOTS" aur "pick by SLOT     │
//  │       NUMBER" — par slot ka concept tha hi nahi!                         │
//  │    2. Ek type ka saamaan sirf EK jagah ho sakta tha. Asli machine me     │
//  │       Coke do slots me ho sakta hai (zyada demand) — ye possible hi      │
//  │       nahi tha.                                                          │
//  │                                                                          │
//  │  Ab: `map<int slotNumber, Slot>` — har slot apna. Ek hi item do slots me │
//  │  ho sakta hai, aur user slot number se chunta hai. ✅                     │
//  │                                                                          │
//  │  💡 `map` use kiya (`unordered_map` nahi) — kyunki slots ko NUMBER ORDER │
//  │     me dikhana hai (101, 102, 103...). `unordered_map` random order deta,│
//  │     aur inventory print ulta-seedha dikhta. Yahan speed se zyada ORDER   │
//  │     matter karta hai (slots to 10-20 hi honge).                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 SINGLE RESPONSIBILITY: Inventory ko paise ka kuch pata nahi (wo
//     MoneyManager ka kaam), aur machine ke state ka bhi nahi (wo States ka
//     kaam). Ye sirf stock jaanta hai. Isi liye ise akele test karna aasan hai.
// ============================================================================
#ifndef VENDING_MACHINE_LLD_SERVICES_INVENTORY_H
#define VENDING_MACHINE_LLD_SERVICES_INVENTORY_H

#include <bits/stdc++.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdexcept>

#include "../enums/ItemType.h"
#include "../models/Item.h"
#include "../models/Slot.h"

using namespace std;

namespace vending_machine_lld {

class Inventory {
public:
    // ========================================================================
    //  ADMIN ke kaam (Requirement R4)
    //  "The admin can put new products in the machine or take them out."
    // ========================================================================

    // ---- Slot me naya product bharo ----------------------------------------
    //
    // Agar slot me PEHLE SE wahi product hai -> bas quantity badha do.
    // Agar khaali hai ya doosra product hai -> naya product set kar do.
    //
    // ⚠ Dhyaan: doosra product OVERWRITE ho jaayega. Ye jaan-boojh ke hai —
    // admin ko haq hai ki slot 101 me Coke ki jagah Pepsi rakh de. Par ye
    // chup-chaap nahi hota, message print hota hai taaki galti se na ho.
    void addProduct(int slotNumber, const Item &item, int quantity) {
        if (quantity <= 0) {
            throw runtime_error("Quantity positive honi chahiye (mili: " + to_string(quantity) + ")");
        }

        auto it = slots_.find(slotNumber);

        // Slot me pehle se WAHI item hai? -> sirf stock badha do
        if (it != slots_.end() && it->second.isOccupied() && it->second.getItem().getType() == item.getType()) {
            it->second.addQuantity(quantity);
            cout << "  [ADMIN] Slot " << slotNumber << " me " << quantity << " aur " << item.getName()
                 << " bhare (ab kul " << it->second.getQuantity() << ")\n";
            return;
        }

        // Doosra product tha? -> user ko batao ki replace ho raha hai
        if (it != slots_.end() && it->second.isOccupied()) {
            cout << "  [ADMIN] ⚠ Slot " << slotNumber << " me pehle " << it->second.getItem().getName()
                 << " tha — ab " << item.getName() << " se replace ho raha hai\n";
        }

        slots_[slotNumber] = Slot(slotNumber, item, quantity);
        cout << "  [ADMIN] ✅ Slot " << slotNumber << " me " << quantity << " x " << item.getName() << " (₹"
             << fixed << setprecision(2) << item.getPrice() << ") bhar diya\n";
    }

    // ---- Slot poora khaali karo (R4 — "take them out") ---------------------
    void removeProduct(int slotNumber) {
        auto it = slots_.find(slotNumber);
        if (it == slots_.end() || !it->second.isOccupied()) {
            throw runtime_error("Slot " + to_string(slotNumber) + " pehle se khaali hai");
        }
        cout << "  [ADMIN] 🗑 Slot " << slotNumber << " se " << it->second.getItem().getName() << " ("
             << it->second.getQuantity() << " pieces) nikaal diya\n";
        it->second.clear();
    }

    // ========================================================================
    //  USER ke kaam (Requirement R5)
    // ========================================================================

    // Slot number valid hai? (yaani usme kuch bhara hai)
    bool isValidSlot(int slotNumber) const {
        auto it = slots_.find(slotNumber);
        return it != slots_.end() && it->second.isOccupied();
    }

    // Us slot me stock bacha hai?
    bool hasStock(int slotNumber) const {
        auto it = slots_.find(slotNumber);
        return it != slots_.end() && it->second.hasStock();
    }

    // ---- Slot padho ---------------------------------------------------------
    //
    // ⭐ Do alag error messages — aur ye jaan-boojh ke hai:
    //    "invalid slot"  -> aisa koi khaana hai hi nahi (galat number dabaya)
    //    "out of stock"  -> khaana to hai, par khatam ho gaya
    // User ke liye ye farak matter karta hai! Pehle case me wo doosra number
    // dabayega; doosre case me wo samajh jaayega ki cheez khatam hai.
    // 📌 Error message me "kya galat hua" saaf hona chahiye, bas "error" nahi.
    const Slot &getSlot(int slotNumber) const {
        auto it = slots_.find(slotNumber);
        if (it == slots_.end() || !it->second.isOccupied()) {
            throw runtime_error("Slot " + to_string(slotNumber) + " valid nahi hai (aisa koi khaana nahi)");
        }
        return it->second;
    }

    // ---- Ek saamaan nikla (bik gaya) ---------------------------------------
    void dispenseFromSlot(int slotNumber) {
        auto it = slots_.find(slotNumber);
        if (it == slots_.end() || !it->second.isOccupied()) {
            throw runtime_error("Slot " + to_string(slotNumber) + " valid nahi hai");
        }
        if (!it->second.hasStock()) {
            throw runtime_error(it->second.getItem().getName() + " out of stock hai");
        }
        it->second.reduceQuantity();
    }

    // ---- Poora stock dikhao (Requirement #9 — base) -------------------------
    void printInventory() const {
        cout << "\n=== 🏪 Machine ka Stock ===\n";
        cout << "Slot | Product          | Price  | Qty\n";
        cout << "-----+------------------+--------+-----\n";

        for (const auto &entry : slots_) {
            const Slot &slot = entry.second;
            if (!slot.isOccupied()) {
                continue; // khaali khaane mat dikhao
            }
            const Item &item = slot.getItem();
            cout << " " << setw(3) << slot.getSlotNumber() << " | " << setw(16) << left << item.getName() << right
                 << " | " << setw(6) << fixed << setprecision(2) << item.getPrice() << " | " << setw(3)
                 << slot.getQuantity();
            if (slot.getQuantity() == 0) {
                cout << "  ❌ OUT OF STOCK";
            }
            cout << "\n";
        }
        cout << "\n";
    }

private:
    // slot number -> Slot. `map` isliye taaki order me rahe (upar note padho).
    map<int, Slot> slots_;
};

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_SERVICES_INVENTORY_H
