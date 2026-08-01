// ============================================================================
//  models/Airport.h  —  Ek airport (source/destination ke liye)
// ----------------------------------------------------------------------------
//  Sirf 3 field ka simple data holder: airport ka code, poora naam, aur city.
//  Flights inhi codes se source/destination batati hain (jaise "DEL" -> "BOM").
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `class` nahi, `struct` kyun?                                          │
//  │                                                                          │
//  │  Ye ek "value object" / DTO hai — sirf data carry karta hai, koi logic    │
//  │  nahi, koi invariant nahi jise chhupana pade. Aise plain data holders ke  │
//  │  liye `struct` (sab public) sahi hai — getters/setters ka boilerplate     │
//  │  bekaar hota. Poore models folder me yahi soch hai.                       │
//  │                                                                          │
//  │  📌 `code` (jaise "DEL") ko system unordered_map me KEY ki tarah use      │
//  │     karta hai — isi liye har airport ka code unique hona chahiye.         │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_AIRPORT_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_AIRPORT_H

#include <string>

namespace airline_mgmt {

struct Airport {
    std::string code; // "DEL" — unique identifier (map me key)
    std::string name; // "Indira Gandhi International"
    std::string city; // "Delhi"
};

} // namespace airline_mgmt

#endif
