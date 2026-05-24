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
