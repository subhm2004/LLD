#ifndef COR_ATM_HANDLERS_TWO_HUNDRED_HANDLER_H
#define COR_ATM_HANDLERS_TWO_HUNDRED_HANDLER_H

#include <iostream>

#include "MoneyHandler.h"

namespace cor_atm {

class TwoHundredHandler : public MoneyHandler {
private:
    int numNotes;

public:
    explicit TwoHundredHandler(int numNotes) : numNotes(numNotes) {}

    void dispense(int amount) override {
        int notesNeeded = amount / 200;

        if (notesNeeded > numNotes) {
            notesNeeded = numNotes;
            numNotes = 0;
        } else {
            numNotes -= notesNeeded;
        }

        if (notesNeeded > 0)
            std::cout << "Dispensing " << notesNeeded << " x ₹200 notes.\n";

        int remainingAmount = amount - (notesNeeded * 200);
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

#endif // COR_ATM_HANDLERS_TWO_HUNDRED_HANDLER_H
