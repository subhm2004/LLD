// ============================================================================
//  strategies/UpiPaymentStrategy.h  —  CONCRETE: UPI se payment
// ----------------------------------------------------------------------------
//  Do validation, phir success:
//     1. amount positive ho
//     2. UPI id khaali na ho
//  Phir ek mock transaction id bana ke SUCCESS.
//
//  ⚠ Ye MOCK gateway hai — asli UPI/PSP integration out of scope. Yahan focus
//     LLD flow pe hai (strategy swap, result handling), na ki real payment API.
//     Asli system me yahan network call, retry, timeout, webhook sab hota.
//
//  📌 Har strategy apna PREFIX wala txn id deta hai (UPI_TXN_ / CARD_TXN_ / COD_)
//     — isse baad me pata chalta hai kaunse channel se paisa aaya (reconciliation).
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_UPIPAYMENTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_UPIPAYMENTSTRATEGY_H

#include <string>

#include "IPaymentStrategy.h"

namespace ecommerce_checkout_lld {

using namespace std;

class UpiPaymentStrategy : public IPaymentStrategy {
public:
    PaymentResult pay(double amount, const string &paymentInstrument) const override {
        if (amount <= 0) {
            return PaymentResult(PaymentStatus::FAILED, "", "Invalid amount");
        }
        if (paymentInstrument.empty()) { // UPI me id zaroori hai
            return PaymentResult(PaymentStatus::FAILED, "", "UPI ID required");
        }
        // Mock txn id (amount ke paise se) — real me gateway deta hai.
        const string txnId = "UPI_TXN_" + to_string(static_cast<long long>(amount * 100));
        return PaymentResult(PaymentStatus::SUCCESS, txnId, "Paid via UPI");
    }
};

}  // namespace ecommerce_checkout_lld

#endif
