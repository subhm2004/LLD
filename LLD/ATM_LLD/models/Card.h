// ============================================================================
//  models/Card.h  —  ATM card (jise machine me daalte ho)
// ----------------------------------------------------------------------------
//  Ek card teen cheezein rakhta hai: apna number, ek PIN (secret), aur wo
//  bank account jisse ye juda hua hai. Card se hi authentication hoti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CARD aur ACCOUNT ek cheez NAHI hain — dhyaan se                       │
//  │                                                                          │
//  │     CARD    -> plastic ka tukda (number + PIN) — "pehchaan"               │
//  │     ACCOUNT -> jahan paisa pada hai (balance) — "khaata"                  │
//  │                                                                          │
//  │  Card sirf account ka NUMBER rakhta hai (`accountNumber_`), poora account │
//  │  nahi. Ek account ke do card ho sakte hain (tumhara + family). Isi liye   │
//  │  alag models: card se account ka reference milta hai, phir system asli    │
//  │  account nikaalta hai. (BankAccount.h me poora note hai.)                 │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `validatePin()` — PIN CHECK CARD KE ANDAR hota hai, bahar nahi
//     User ka PIN card khud se compare karta hai. Isse `pin_` PRIVATE reh jaata
//     hai — bahar koi getPin() nahi hai. Ye "secret ko andar hi rakho, comparison
//     bhi andar karo" wali soch security ke liye achhi hai (leak nahi hota).
//
//  ⚠ Real system me PIN plain text me nahi, HASH karke store hota hai (aur
//     compare bhi hash se). Yahan demo hai isliye plain — interview me ye
//     limitation bolna acchha point hai.
// ============================================================================
#ifndef ATM_LLD_MODELS_CARD_H
#define ATM_LLD_MODELS_CARD_H

#include <string>
#include <utility>

using namespace std;

namespace atm_lld {

class Card {
public:
    // Card number + PIN + linked account number — teeno set.
    Card(string cardNumber, string pin, string accountNumber)
        : cardNumber_(std::move(cardNumber)), pin_(std::move(pin)), accountNumber_(std::move(accountNumber)) {}

    const string &getCardNumber() const { return cardNumber_; }
    const string &getAccountNumber() const { return accountNumber_; } // account ka reference

    // ⭐ PIN comparison card ke andar — `pin_` bahar leak nahi hota.
    // true agar entered PIN sahi hai.
    bool validatePin(const string &enteredPin) const {
        return enteredPin == pin_;
    }

private:
    string cardNumber_;    // unique card id
    string pin_;           // secret PIN (private — koi getter nahi, sirf validate)
    string accountNumber_; // kis account se juda hai
};

} // namespace atm_lld

#endif // ATM_LLD_MODELS_CARD_H
