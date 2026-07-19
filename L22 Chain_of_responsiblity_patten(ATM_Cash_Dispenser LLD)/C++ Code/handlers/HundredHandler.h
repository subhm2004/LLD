// ============================================================================
//  handlers/HundredHandler.h — AAKHRI handler (₹100 notes, chain ka end)
// ----------------------------------------------------------------------------
//  Same 4-step dispense pattern, denomination 100. Ye chain ka LAST link hai
//  (nextHandler = nullptr). Agar iske baad bhi amount bacha to "Insufficient
//  fund" — ATM ke paas sahi combination ke notes nahi the. Chain khatam.
//  Chain: ... TwoHundred -> [Hundred] -> (end).
// ============================================================================
#ifndef COR_ATM_HANDLERS_HUNDRED_HANDLER_H
#define COR_ATM_HANDLERS_HUNDRED_HANDLER_H

#include <iostream>

#include "MoneyHandler.h"

namespace cor_atm {

class HundredHandler : public MoneyHandler {
private:
    int numNotes;

public:
    explicit HundredHandler(int numNotes) : numNotes(numNotes) {}

    void dispense(int amount) override {
        int notesNeeded = amount / 100;

        if (notesNeeded > numNotes) {
            notesNeeded = numNotes;
            numNotes = 0;
        } else {
            numNotes -= notesNeeded;
        }

        if (notesNeeded > 0)
            std::cout << "Dispensing " << notesNeeded << " x ₹100 notes.\n";

        int remainingAmount = amount - (notesNeeded * 100);
        if (remainingAmount > 0) {
            if (nextHandler != nullptr)
                nextHandler->dispense(remainingAmount);
            else {
                std::cout << "Remaining amount of " << remainingAmount
                          << " cannot be fulfilled (Insufficinet fund in ATM)\n";
            }
        }
    }
};

} // namespace cor_atm

#endif // COR_ATM_HANDLERS_HUNDRED_HANDLER_H
