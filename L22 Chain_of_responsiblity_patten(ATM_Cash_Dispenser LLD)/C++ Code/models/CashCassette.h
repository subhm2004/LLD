// models/CashCassette.h — Ek denomination ka cassette (drawer): note value +
// available count. Inventory inhi cassettes se banti hai.
#ifndef COR_ATM_MODELS_CASH_CASSETTE_H
#define COR_ATM_MODELS_CASH_CASSETTE_H

#include "../enums/NoteDenomination.h"

namespace cor_atm {

class CashCassette {
public:
    CashCassette(NoteDenomination denomination, int numNotes)
        : denomination_(denomination), numNotes_(numNotes) {}

    NoteDenomination getDenomination() const { return denomination_; }
    int getNumNotes() const { return numNotes_; }
    void setNumNotes(int numNotes) { numNotes_ = numNotes; }

private:
    NoteDenomination denomination_;
    int numNotes_;
};

} // namespace cor_atm

#endif // COR_ATM_MODELS_CASH_CASSETTE_H
