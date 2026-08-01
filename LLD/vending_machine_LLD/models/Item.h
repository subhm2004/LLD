// ============================================================================
//  models/Item.h  —  Ek product (Coke, Pepsi, Water...)
// ----------------------------------------------------------------------------
//  Sirf teen cheezein: naam, daam, aur type. Bas.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ITEM aur SLOT ka farak — ye samajhna zaroori hai                     │
//  │                                                                          │
//  │     ITEM -> "Coke Can, ₹40, type COKE"   (product KYA hai)               │
//  │     SLOT -> "khaana 101, isme Coke hai, 3 bache"  (KAHAN hai, KITNE hain)│
//  │                                                                          │
//  │  Isi liye Item me `quantity` NAHI hai! Quantity slot ki cheez hai, item  │
//  │  ki nahi. Ek hi item (Coke) do slots me ho sakta hai — dono me alag      │
//  │  quantity, alag daam bhi. (Slot.h me poora explanation hai.)             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Ye IMMUTABLE hai — koi setter nahi. Ek baar Item ban gaya, uska naam ya
//  daam badal nahi sakta. Ye jaan-boojh ke hai:
//     - Item slots me copy hoke jaata hai, aur agar koi beech me uska daam
//       badal deta to bahut confusion hota (kis slot ka daam badla?)
//     - Daam badalna ho? Admin naya Item bana ke slot me daal de (R4).
//  📌 Value objects (jinme sirf data ho, koi behavior nahi) ko immutable rakhna
//     ek acchi aadat hai — unhe copy karna, share karna, sab safe ho jaata hai.
//
//  💡 Ye ek "value object" / DTO hai — sirf data carry karta hai, koi logic
//     nahi. Isi liye ise slots me POINTER se nahi, VALUE se rakhte hain
//     (`Item item_`, `Item* item_` nahi) — chhota object hai, `new`/`delete`
//     ki jhanjhat bekaar hai.
// ============================================================================
#ifndef VENDING_MACHINE_LLD_MODELS_ITEM_H
#define VENDING_MACHINE_LLD_MODELS_ITEM_H

#include <bits/stdc++.h>
#include <string>
#include <utility>

#include "../enums/ItemType.h"

using namespace std;

namespace vending_machine_lld {

class Item {
public:
    // ---- Default constructor — khaali/placeholder item ---------------------
    //
    // Ye kyun chahiye? Kyunki `Slot` ka default constructor ek khaali Slot
    // banata hai, aur usme ek Item to hona hi padega (member value me hai).
    // Aur `map<int, Slot>` bhi `slots_[key]` pe default Slot banata hai.
    //
    // ⚠ Iska matlab ek "khaali" Item ban sakta hai (naam "", daam 0). Kya ye
    // khatarnak hai? Nahi — kyunki `Slot::occupied_` false hota hai aise slots
    // me, aur Inventory unhe kabhi user ko dikhata/dispense nahi karta.
    // (Slot.h me `occupied_` wala note padho.)
    Item() : name_(""), price_(0.0), type_(ItemType::WATER) {}

    // `std::move(name)` — string ki COPY nahi banti, uska andar ka data seedha
    // "cheen" (move) liya jaata hai. Param by-value liya hai isi liye safe hai.
    Item(std::string name, double price, ItemType type)
        : name_(std::move(name)), price_(price), type_(type) {}

    // ---- Sirf getters — koi setter nahi (immutable, upar note padho) -------
    //
    // `const std::string &` return karta hai (`std::string` nahi) — har call pe
    // poori string copy hone se bach jaati hai. Chhoti optimization, par getters
    // me ye aadat achhi hai.
    const std::string &getName() const { return name_; }
    double getPrice() const { return price_; }
    ItemType getType() const { return type_; }

private:
    std::string name_; // "Coke Can" — user ko dikhne wala naam
    double price_;     // ₹40.00 (R8/R9/R10 isi se compare karte hain)
    ItemType type_;    // COKE/PEPSI/WATER/JUICE — category
};

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_MODELS_ITEM_H
