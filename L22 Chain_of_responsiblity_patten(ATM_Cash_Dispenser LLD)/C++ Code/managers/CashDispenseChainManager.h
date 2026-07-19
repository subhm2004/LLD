// ============================================================================
//  managers/CashDispenseChainManager.h — chain ka BUILDER + OWNER
// ----------------------------------------------------------------------------
//  Ye class handlers ki chain ek jagah WIRE karti hai (Thousand -> FiveHundred
//  -> TwoHundred -> Hundred) — order EK jagah define, galat links ka risk nahi.
//  Inventory se har handler ka note-count leti hai (kitne notes available).
//
//  ⭐ OWNERSHIP + CLEANUP (monolith se bada improvement):
//  chainHead_ pointer rakhta hai; destroyChain() `delete chainHead_` karta
//  hai. Pehle isme ek LEAK tha (sirf pehla handler delete hota, baaki 3
//  leak), par ab MoneyHandler ka destructor RECURSIVE hai (apne next ko
//  delete karta hai) -> head delete = poori chain saaf. buildChain() bhi
//  pehle destroyChain() call karta hai taaki re-init pe purani chain leak na ho.
//  Monolith (COR_Original.cpp) me to cleanup tha hi nahi — ye modular
//  version usse behtar hai.
// ============================================================================
#ifndef COR_ATM_MANAGERS_CASH_DISPENSE_CHAIN_MANAGER_H
#define COR_ATM_MANAGERS_CASH_DISPENSE_CHAIN_MANAGER_H

#include "../handlers/FiveHundredHandler.h"
#include "../handlers/HundredHandler.h"
#include "../handlers/ThousandHandler.h"
#include "../handlers/TwoHundredHandler.h"
#include "../models/ATMCashInventory.h"

namespace cor_atm {

// Builds and owns the handler chain (₹1000 → ₹500 → ₹200 → ₹100)
class CashDispenseChainManager {
public:
    MoneyHandler *buildChain(const ATMCashInventory &inventory) {
        destroyChain();

        MoneyHandler *thousandHandler =
            new ThousandHandler(inventory.getNumNotes(NoteDenomination::THOUSAND));
        MoneyHandler *fiveHundredHandler =
            new FiveHundredHandler(inventory.getNumNotes(NoteDenomination::FIVE_HUNDRED));
        MoneyHandler *twoHundredHandler =
            new TwoHundredHandler(inventory.getNumNotes(NoteDenomination::TWO_HUNDRED));
        MoneyHandler *hundredHandler =
            new HundredHandler(inventory.getNumNotes(NoteDenomination::HUNDRED));

        thousandHandler->setNextHandler(fiveHundredHandler);
        fiveHundredHandler->setNextHandler(twoHundredHandler);
        twoHundredHandler->setNextHandler(hundredHandler);

        chainHead_ = thousandHandler;
        return chainHead_;
    }

    MoneyHandler *getChainHead() const { return chainHead_; }

    void destroyChain() {
        delete chainHead_;
        chainHead_ = nullptr;
    }

    ~CashDispenseChainManager() { destroyChain(); }

private:
    MoneyHandler *chainHead_ = nullptr;
};

} // namespace cor_atm

#endif // COR_ATM_MANAGERS_CASH_DISPENSE_CHAIN_MANAGER_H
