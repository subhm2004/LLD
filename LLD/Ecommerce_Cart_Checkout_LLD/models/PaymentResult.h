// ============================================================================
//  models/PaymentResult.h  —  Payment ka natija (status + txnId + reason)
// ----------------------------------------------------------------------------
//  IPaymentStrategy::pay() yahi lauta ta hai. Teen cheezein:
//     status        -> SUCCESS / FAILED
//     transactionId -> success pe gateway ka reference ("UPI_TXN_...")
//     message       -> insaan-padhne-layak reason ("Card declined by bank")
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ RESULT OBJECT kyun (bool ya exception ke bajaye)?                     │
//  │                                                                          │
//  │  - `bool` hota to sirf haan/na pata chalta — "kyun fail hua" aur txn id   │
//  │    dono kho jaate.                                                       │
//  │  - Exception phenkte to "payment fail hona" ko EXCEPTIONAL treat karna    │
//  │    padta — par card decline hona bilkul NORMAL business case hai (roz     │
//  │    hota hai), koi system fault nahi.                                     │
//  │                                                                          │
//  │  Result object se caller ko poori jaankari milti hai aur wo smart decision│
//  │  le sakta hai: user ko exact reason dikhao, txnId order me save karo.    │
//  │  📌 Ye "errors as values" wala approach hai (exceptions ke saath-saath). │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Default `status_{FAILED}` — safe default. Agar galti se status set na ho
//     to system "fail" maanega (na ki galti se "success" — jo paisa-related
//     code me khatarnak hota). "Fail closed, not open."
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_PAYMENTRESULT_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_PAYMENTRESULT_H

#include <string>
#include <utility>

#include "../enums/PaymentStatus.h"

namespace ecommerce_checkout_lld {

using namespace std;

class PaymentResult {
public:
    PaymentResult(PaymentStatus status, string transactionId, string message)
        : status_(status),
          transactionId_(std::move(transactionId)),
          message_(std::move(message)) {}

    PaymentStatus getStatus() const { return status_; }
    const string &getTransactionId() const { return transactionId_; } // success pe gateway ref
    const string &getMessage() const { return message_; }             // fail pe reason
    bool isSuccess() const { return status_ == PaymentStatus::SUCCESS; }

private:
    PaymentStatus status_{PaymentStatus::FAILED}; // ⭐ safe default (fail closed)
    string transactionId_;
    string message_;
};

}  // namespace ecommerce_checkout_lld

#endif
