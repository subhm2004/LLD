// ============================================================================
//  enums/ItemType.h  —  Product ki category
// ----------------------------------------------------------------------------
//  Machine me kis TARAH ka saamaan hai — COKE, PEPSI, WATER, JUICE.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Pehle ye enum ITEMS ki PEHCHAAN thi — ab sirf CATEGORY hai           │
//  │                                                                          │
//  │  Purana code items ko is enum se dhoondhta tha:                          │
//  │      unordered_map<ItemType, Item> items_;   // COKE -> Coke Can         │
//  │      machine.selectItem(ItemType::COKE);                                 │
//  │                                                                          │
//  │  Problem: ek TYPE ka saamaan sirf EK jagah ho sakta tha! Asli machine me │
//  │  Coke do slots me ho sakta hai (zyada demand wali cheez do khaanon me).  │
//  │  Aur R1/R5 kehte hain "products in various SLOTS", "pick by SLOT NUMBER" │
//  │  — par slot ka concept tha hi nahi.                                      │
//  │                                                                          │
//  │  Ab: slots se pehchaan hoti hai (Slot.h), aur ye enum sirf "ye cheez     │
//  │  kaunsi category ki hai" batata hai. Slot 101 me Coke ₹40 ka, slot 105   │
//  │  me Coke ₹45 ka — dono ka type COKE, par alag slot aur alag daam. ✅      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  💡 To ab is enum ka kaam kya hai?
//     - `Inventory::addProduct` isse check karta hai ki slot me "wahi product"
//       hai ya "doosra product" (agar wahi -> stock badha do; doosra -> replace)
//     - Aage chal ke reporting/analytics ke liye ("aaj kitni COKE bikhi?")
//     - Category-wise discount/tax lagane ke liye
//
//  ⚠ Naya product add karna ho (jaise CHIPS) to yahan bhi entry karni padegi
//     aur `itemTypeToString` me bhi. Ye enum ki kami hai — wo "closed" hota hai.
//     Bahut zyada types hon to enum ki jagah ek `Category` class better hoti
//     (tab naye types bina code chhue add ho sakte). Yahan 4 hi hain, to enum
//     bilkul theek hai. 📌 Design me "abhi kitna chahiye" dekho, "kabhi kya ho
//     sakta hai" nahi.
//
//  `enum class` (scoped) hai — `ItemType::COKE` likhna padta hai (naam bahar
//  leak nahi hota), aur `int` me chupke se convert nahi hota. Type safety free.
//  (`UserRole.h` me bhi yahi kiya hai — poore codebase me consistent rehna
//   acchi aadat hai.)
// ============================================================================
#ifndef VENDING_MACHINE_LLD_ENUMS_ITEMTYPE_H
#define VENDING_MACHINE_LLD_ENUMS_ITEMTYPE_H

#include <bits/stdc++.h>
#include <string>

using namespace std;

namespace vending_machine_lld {

enum class ItemType { COKE, PEPSI, WATER, JUICE };

// Enum ko padhne layak string me — printing/inventory ke liye.
// `inline` isliye taaki ye header kai .cpp me include ho to linker "duplicate
// symbol" ka error na de.
inline std::string itemTypeToString(ItemType type) {
    if (type == ItemType::COKE) {
        return "COKE";
    }
    if (type == ItemType::PEPSI) {
        return "PEPSI";
    }
    if (type == ItemType::WATER) {
        return "WATER";
    }
    return "JUICE";
}

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_ENUMS_ITEMTYPE_H
