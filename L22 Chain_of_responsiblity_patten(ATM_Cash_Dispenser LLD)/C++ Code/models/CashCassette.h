// ============================================================================
//  models/CashCassette.h — Ek denomination ka CASSETTE (drawer) 🗄️
// ----------------------------------------------------------------------------
//  Asli ATM me har note-value ka apna physical cassette (tray) hota hai.
//  Ye model wahi represent karta hai: kaunsa denomination + kitne notes.
//  Inventory in cassettes ka collection hai. setNumNotes() se count update
//  ho sakta hai (dispense ke baad ghatana). Real-world modeling ka achha
//  example — monolith me sirf `int numNotes` tha, yahan proper model.
// ============================================================================
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
