// ============================================================================
//  models/WithdrawalRequest.h — User ka withdrawal request (kitna nikalna hai)
// ----------------------------------------------------------------------------
//  Chhota immutable data holder — bas amount. Ye "request" object CoR ka
//  classic hissa hai: request ko ek object banake chain me bhejte hain.
//  Service isse leta hai, validate karta hai, phir amount chain me daalta hai.
//  (Abhi sirf amount; future me account-id, currency, timestamp add ho sakte.)
// ============================================================================
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
