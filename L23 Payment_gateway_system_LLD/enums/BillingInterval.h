// enums/BillingInterval.h — Subscription billing kab repeat ho (jaise MONTHLY/
// YEARLY). RecurringPaymentService isse billing cycle decide karti hai.
#ifndef PAYMENT_GATEWAY_LLD_ENUMS_BILLINGINTERVAL_H
#define PAYMENT_GATEWAY_LLD_ENUMS_BILLINGINTERVAL_H

namespace payment_gateway_lld {
enum class BillingInterval { WEEKLY, MONTHLY };
}

#endif
