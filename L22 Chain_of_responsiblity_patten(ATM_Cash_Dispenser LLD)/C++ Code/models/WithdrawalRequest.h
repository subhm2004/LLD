// models/WithdrawalRequest.h — User ka withdrawal request: kitna amount nikalna
// hai (+ remaining track). Yeh request handlers ki chain me se guzarta hai.
#ifndef COR_ATM_MODELS_WITHDRAWAL_REQUEST_H
#define COR_ATM_MODELS_WITHDRAWAL_REQUEST_H

namespace cor_atm {

class WithdrawalRequest {
public:
    explicit WithdrawalRequest(int amount) : amount_(amount) {}

    int getAmount() const { return amount_; }

private:
    int amount_;
};

} // namespace cor_atm

#endif // COR_ATM_MODELS_WITHDRAWAL_REQUEST_H
