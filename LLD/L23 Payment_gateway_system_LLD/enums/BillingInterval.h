// ============================================================================
//  enums/BillingInterval.h — Subscription billing frequency: WEEKLY / MONTHLY
// ----------------------------------------------------------------------------
//  Subscription kitni baar charge ho. RecurringPaymentService isse billing
//  cycle me use karta (abhi mostly label/data ke roop me). Type-safe enum —
//  naya interval (YEARLY, DAILY) easily add ho sakta.
// ============================================================================
#ifndef PAYMENT_GATEWAY_LLD_ENUMS_BILLINGINTERVAL_H
#define PAYMENT_GATEWAY_LLD_ENUMS_BILLINGINTERVAL_H

namespace payment_gateway_lld {
enum class BillingInterval { WEEKLY, MONTHLY };
}

#endif
