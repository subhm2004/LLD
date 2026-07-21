// ============================================================================
//  types/SegmentLedger.h  —  Ek train run ka "seat -> bookings" register
// ----------------------------------------------------------------------------
//  Ek train run par kaunsi seat pe kaunse-kaunse SEGMENTS book hue, uska hisaab.
//  Structure: seatId -> us seat pe hui saari bookings (segments) ki list.
//
//     ledger["S1"] = [ {Alice, BOM->BRC [0,2)}, {Bob, RTM->NDLS [3,6)} ]
//     ledger["S2"] = [ {Carol, BOM->NDLS [0,5)} ]
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye class `unordered_map` ko INHERIT karti hai (typedef nahi) — kyun?   │
//  │                                                                          │
//  │  `class SegmentLedger : public unordered_map<string, vector<TicketBooking>>` │
//  │                                                                          │
//  │  Ek MEANINGFUL naam mil gaya ("SegmentLedger") jo intent batata hai —     │
//  │  raw `unordered_map<string, vector<TicketBooking>>` har jagah likhne se    │
//  │  behtar (padhne me clear, aur function signatures chhote). Aur inherit     │
//  │  isliye taaki existing `ledger[key]` / `ledger.find()` code jaisa hai      │
//  │  waisa chalta rahe (map ke saare operations available).                   │
//  │                                                                          │
//  │  ⚠ Note: STL containers ko inherit karna aam tor pe avoid karte hain      │
//  │  (unka destructor virtual nahi hota — base pointer se delete UB). Yahan   │
//  │  safe hai kyunki hum SegmentLedger* ko base pointer me store/delete nahi   │
//  │  karte (bas value ki tarah use karte). Interview me ye caveat bolna accha.│
//  │  (Alternative: `using SegmentLedger = unordered_map<...>;` bhi chalta.)   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `using Map::Map;` = base ke saare constructors inherit karo (taaki
//     SegmentLedger ko map ki tarah initialize kar sakein).
// ============================================================================
#ifndef IRCTC_LLD_TYPES_SEGMENTLEDGER_H
#define IRCTC_LLD_TYPES_SEGMENTLEDGER_H

#include <string>
#include <unordered_map>
#include <vector>

#include "../models/TicketBooking.h"

namespace irctc_lld {

class SegmentLedger : public std::unordered_map<std::string, std::vector<TicketBooking>> {
public:
    using Map = std::unordered_map<std::string, std::vector<TicketBooking>>;
    using Map::Map; // base ke constructors inherit karo
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_TYPES_SEGMENTLEDGER_H
