// ============================================================================
//  enums/NoteDenomination.h — Note ki value: 1000 / 500 / 200 / 100
// ----------------------------------------------------------------------------
//  Har handler ek denomination ko represent karta hai (chain ka order isi
//  se: bade se chhote). CLEVER: enum values ACTUAL note-value hain
//  (THOUSAND = 1000), isliye denominationValue() bas static_cast karke
//  int nikaal leta hai — koi switch/lookup nahi chahiye! ATMCashInventory
//  ka custom hash bhi isi int value pe kaam karta hai.
// ============================================================================
#ifndef COR_ATM_ENUMS_NOTE_DENOMINATION_H
#define COR_ATM_ENUMS_NOTE_DENOMINATION_H

namespace cor_atm {

enum class NoteDenomination {
    HUNDRED = 100,
    TWO_HUNDRED = 200,
    FIVE_HUNDRED = 500,
    THOUSAND = 1000
};

inline int denominationValue(NoteDenomination denomination) {
    return static_cast<int>(denomination);
}

} // namespace cor_atm

#endif // COR_ATM_ENUMS_NOTE_DENOMINATION_H
