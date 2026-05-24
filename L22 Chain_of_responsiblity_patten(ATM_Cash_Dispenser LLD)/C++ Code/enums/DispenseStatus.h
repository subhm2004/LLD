#ifndef COR_ATM_ENUMS_DISPENSE_STATUS_H
#define COR_ATM_ENUMS_DISPENSE_STATUS_H

namespace cor_atm {

enum class DispenseStatus {
    SUCCESS,
    INVALID_AMOUNT,
    INSUFFICIENT_ATM_CASH
};

} // namespace cor_atm

#endif // COR_ATM_ENUMS_DISPENSE_STATUS_H
