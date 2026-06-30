// managers/CashDispenseChainManager.h — Handlers ki chain ko WIRE karta hai:
// Thousand -> FiveHundred -> TwoHundred -> Hundred. Ek hi jagah chain banती hai,
// taaki order/links galat na hon. Service isi se dispense start karti hai.
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
