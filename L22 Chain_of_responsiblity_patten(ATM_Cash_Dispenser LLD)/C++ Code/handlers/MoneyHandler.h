// handlers/MoneyHandler.h — Chain of Responsibility ka BASE handler. Har handler
// apni denomination ke notes dispense karta hai aur bacha hua amount NEXT handler
// ko pass karta hai (setNextHandler). Yahi CoR ki reedh ki haddi hai.
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
    virtual ~MoneyHandler() = default;
};

} // namespace cor_atm

#endif // COR_ATM_HANDLERS_MONEY_HANDLER_H
