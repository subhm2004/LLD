// ============================================================================
//  config/SystemLimits.h  —  Poore system ke NIYAM (limits) ek hi jagah
// ----------------------------------------------------------------------------
//  Ye file kuch "karti" nahi — bas building ke rules ek jagah likhti hai.
//  Par ye ek bahut zaroori design aadat hai:
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ MAGIC NUMBERS se bacho — constants ek jagah rakho                    │
//  │                                                                          │
//  │  Bina is file ke code aisa dikhta:                                       │
//  │      if (floor > 15) { ... }          // 15 kya hai? kahan se aaya?      │
//  │      if (people > 8) { ... }          // 8 kyun?                         │
//  │      if (weight > 680) { ... }        // 680 ka matlab?                  │
//  │                                                                          │
//  │  Aise numbers ko "MAGIC NUMBERS" kehte hain — code me pade hote hain     │
//  │  par unka MATLAB kahin likha nahi hota. Do problem:                      │
//  │    1. Padhne wale ko pata nahi chalta ye number kya hai                  │
//  │    2. Kal building me 20 floors ho gaye? Ab poore codebase me "15"       │
//  │       dhoondho aur badlo — aur ek jagah bhoole to CHUP-CHAAP bug 🐛      │
//  │                                                                          │
//  │  Ab: ek jagah badlo, poore system me asar. Aur naam khud batata hai      │
//  │  ki number ka matlab kya hai (`MAX_FLOORS` vs `15`).                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `constexpr` kyun, `#define` kyun nahi?
//     `#define MAX_FLOORS 15` bhi chalta, par wo sirf "text replace" hota hai —
//     usme TYPE nahi hota, namespace nahi hota, aur debugger me dikhta bhi nahi.
//     `constexpr int` ek asli typed constant hai jo compile-time pe hi pata hota
//     hai. Modern C++ me hamesha `constexpr` use karo, `#define` nahi.
// ============================================================================
#ifndef ELEVATOR_SYSTEM_LLD_CONFIG_SYSTEMLIMITS_H
#define ELEVATOR_SYSTEM_LLD_CONFIG_SYSTEMLIMITS_H

namespace elevator_lld {

// ---- Building ke limits (Requirement #8) -----------------------------------

// Building me zyada se zyada kitni floors ho sakti hain.
// Floors 0 se ginti hain (0 = ground floor), to valid floors: 0 se 14 tak.
constexpr int MAX_FLOORS = 15;

// Zyada se zyada kitne elevators in floors ko service kar sakte hain.
constexpr int MAX_ELEVATORS = 3;

// ---- Elevator ki capacity (Requirement #19) --------------------------------
//
// ⭐ DO-TARFA limit hai — dono me se jo PEHLE bhar jaaye:
//     - 8 log baith sakte hain, YA
//     - 680 kg kul weight
//
// Dono kyun? Socho: 8 bachche (har ek 30 kg) = 240 kg -> weight se to bahut
// jagah hai, par log 8 ho gaye -> full. Ulta: 5 bade aadmi (har ek 140 kg) =
// 700 kg -> log to sirf 5 hain, par weight limit paar -> full.
// Isi liye SIRF EK check karna kaafi nahi — dono lagane padte hain.
constexpr int MAX_PEOPLE = 8;
constexpr int MAX_WEIGHT_KG = 680;

// Agar passenger apna weight na bataye, to itna maan lete hain.
// (680 / 8 = 85 — yaani limits aapas me consistent hain: 8 "average" log
//  theek 680 kg banate hain. Ye ittefaq nahi, jaan-boojh ke aisa hai.)
constexpr int DEFAULT_PASSENGER_WEIGHT_KG = 85;

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_CONFIG_SYSTEMLIMITS_H
