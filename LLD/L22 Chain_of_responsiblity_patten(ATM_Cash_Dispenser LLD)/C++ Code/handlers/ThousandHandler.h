// ============================================================================
//  handlers/ThousandHandler.h — Chain ka PEHLA handler (₹1000 notes)
// ----------------------------------------------------------------------------
//  dispense() ka 4-step pattern (saare handlers me yahi, sirf denomination alag):
//    1. notesNeeded = amount/1000 (kitne ₹1000 note chahiye)
//    2. stock check: itne hain? nahi to jitne hain utne do, stock 0 karo
//    3. jo diye print karo
//    4. bacha hua amount NEXT handler (₹500) ko forward -> yahi CoR ka core!
//  Chain ka sabse pehla link — bade note se shuru taaki kam notes me kaam ho.
// ============================================================================
#ifndef COR_ATM_HANDLERS_THOUSAND_HANDLER_H
#define COR_ATM_HANDLERS_THOUSAND_HANDLER_H

#include <iostream>

#include "MoneyHandler.h"

namespace cor_atm {

class ThousandHandler : public MoneyHandler {
private:
    int numNotes;

public:
    explicit ThousandHandler(int numNotes) : numNotes(numNotes) {}

    void dispense(int amount) override {
        int notesNeeded = amount / 1000;

        if (notesNeeded > numNotes) {
            notesNeeded = numNotes;
            numNotes = 0;
        } else {
            numNotes -= notesNeeded;
        }

        if (notesNeeded > 0)
            std::cout << "Dispensing " << notesNeeded << " x ₹1000 notes.\n";

        int remainingAmount = amount - (notesNeeded * 1000);
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

#endif // COR_ATM_HANDLERS_THOUSAND_HANDLER_H
