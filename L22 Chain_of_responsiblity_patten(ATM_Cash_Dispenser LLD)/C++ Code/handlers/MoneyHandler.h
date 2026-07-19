// ============================================================================
//  handlers/MoneyHandler.h — CoR ka BASE handler (chain ki reedh ki haddi)
// ----------------------------------------------------------------------------
//  Har handler ke paas: nextHandler pointer (chain ki agli kadi) + dispense()
//  (pure virtual — har denomination handler apna logic deta hai).
//  setNextHandler() se handlers ek linked chain me jud-te hain. Yahi 2
//  cheezein pura Chain of Responsibility banati hain.
// ============================================================================
#ifndef COR_ATM_HANDLERS_MONEY_HANDLER_H
#define COR_ATM_HANDLERS_MONEY_HANDLER_H

namespace cor_atm {

// Abstract Handler (Base Class) — Chain of Responsibility
class MoneyHandler {
protected:
    MoneyHandler *nextHandler;

public:
    MoneyHandler() : nextHandler(nullptr) {}

    void setNextHandler(MoneyHandler *next) { nextHandler = next; }

    virtual void dispense(int amount) = 0;

    // RECURSIVE CLEANUP FIX: pehle ye `= default` tha, jisse
    // manager ka `delete chainHead_` sirf pehla handler delete karta aur
    // baaki 3 LEAK ho jaate. Ab har handler apne NEXT ko delete karta hai
    // -> head delete karo, poori chain domino ki tarah saaf! (L24 Coupon
    // chain wala hi trick — CoR chains me ye standard cleanup hai.)
    virtual ~MoneyHandler() { delete nextHandler; }
};

} // namespace cor_atm

#endif // COR_ATM_HANDLERS_MONEY_HANDLER_H
