#ifndef ATM_LLD_MODELS_CARD_H
#define ATM_LLD_MODELS_CARD_H

#include <bits/stdc++.h>

using namespace std;

namespace atm_lld {

/**
 * @class Card
 * @brief ATM physical card ko represent karti hai.
 * 
 * Har card par ek unique Card Number hota hai, jo ek unique Bank Account number se map hota hai,
 * aur security authentication ke liye isme PIN code bhi save rehta hai.
 */
class Card {
public:
    // Card card number, security pin aur corresponding bank account map karne ke liye constructor.
    Card(string cardNumber, string pin, string accountNumber)
        : cardNumber_(std::move(cardNumber)), pin_(std::move(pin)), accountNumber_(std::move(accountNumber)) {}

    // Card Number get karne ka getter.
    const string &getCardNumber() const { return cardNumber_; }
    
    // Account Number fetch karne ka getter.
    const string &getAccountNumber() const { return accountNumber_; }
    
    // User ke entered PIN ko internal PIN se validate check karne ka helper method.
    bool validatePin(const string &enteredPin) const { 
        return enteredPin == pin_; 
    }

private:
    string cardNumber_;    // Unique card number
    string pin_;           // Encrypted/Stored PIN security verification code
    string accountNumber_; // Linked bank account number reference
};

} // namespace atm_lld

#endif // ATM_LLD_MODELS_CARD_H
