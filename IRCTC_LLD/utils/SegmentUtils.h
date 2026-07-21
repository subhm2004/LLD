// ============================================================================
//  utils/SegmentUtils.h  —  Do journey segments overlap karte hain kya (CORE math)
// ----------------------------------------------------------------------------
//  Ye ek chhota par POORE SYSTEM ka dil hai: do segments (index intervals) aapas
//  me takra rahe hain ya nahi. Isi se decide hota hai ki ek physical seat dobara
//  bik sakti hai ya nahi.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ HALF-OPEN INTERVALS `[from, to)` — "to" wala station EXCLUDE           │
//  │                                                                          │
//  │  Ek journey BOM(0)->BRC(2) ko interval [0, 2) samajho — matlab passenger  │
//  │  seat pe hai stations 0 aur 1 ke beech (BOM-ST, ST-BRC legs), par BRC(2)  │
//  │  pe UTAR jaata hai. To BRC se aage seat KHAALI hai.                       │
//  │                                                                          │
//  │  Isi liye Bob BRC(2)->NDLS(5) = [2, 5) SAME seat le sakta hai — kyunki    │
//  │  [0,2) aur [2,5) TOUCH karte hain par OVERLAP nahi (2 dono me shared      │
//  │  nahi — pehle me exclude, doosre me include).                            │
//  │                                                                          │
//  │  Formula:  A=[fA,tA), B=[fB,tB) overlap  <=>  fA < tB  AND  fB < tA       │
//  │     [0,2) vs [2,5): 0<5 (haan) AND 2<2 (NAHI) -> overlap = false ✅       │
//  │     [0,2) vs [1,3): 0<3 (haan) AND 1<2 (haan) -> overlap = true  ✅       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Agar CLOSED intervals `[from, to]` use karte, to BRC pe ek passenger utar
//     raha aur doosra chadh raha "conflict" mana jaata (galat). Half-open se ye
//     boundary case sahi handle hota — yahi is problem ka sabse nazuk detail hai.
// ============================================================================
#ifndef IRCTC_LLD_UTILS_SEGMENTUTILS_H
#define IRCTC_LLD_UTILS_SEGMENTUTILS_H

namespace irctc_lld {

// A=[fromA, toA), B=[fromB, toB). Overlap tabhi jab dono taraf se "ghusa" ho.
inline bool segmentsOverlap(int fromA, int toA, int fromB, int toB) {
    return fromA < toB && fromB < toA;
}

}  // namespace irctc_lld

#endif  // IRCTC_LLD_UTILS_SEGMENTUTILS_H
