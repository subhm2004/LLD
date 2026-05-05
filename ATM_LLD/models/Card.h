#ifndef ATM_LLD_MODELS_CARD_H
#define ATM_LLD_MODELS_CARD_H

#include <bits/stdc++.h>

using namespace std;

namespace atm_lld {

class Card {
public:
    Card(string cardNumber, string pin, string accountNumber)
        : cardNumber_(std::move(cardNumber)), pin_(std::move(pin)), accountNumber_(std::move(accountNumber)) {}

    const string &getCardNumber() const { return cardNumber_; }
    const string &getAccountNumber() const { return accountNumber_; }
    bool validatePin(const string &enteredPin) const { return enteredPin == pin_; }

private:
    string cardNumber_;
    string pin_;
    string accountNumber_;
};

} // namespace atm_lld

#endif // ATM_LLD_MODELS_CARD_H
