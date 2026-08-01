// ============================================================================
//  enums/RetryStrategyType.h — Retry policy: LINEAR / EXPONENTIAL_BACKOFF
// ----------------------------------------------------------------------------
//  LINEAR = fixed wait har retry pe; EXPONENTIAL_BACKOFF = badhta wait
//  (100->200->400ms). RetryStrategyFactory isse sahi strategy choose karta.
//  Client (main) ye decide karta payment ke saath (default LINEAR). Type-safe enum.
// ============================================================================
#ifndef PAYMENT_GATEWAY_LLD_ENUMS_RETRYSTRATEGYTYPE_H
#define PAYMENT_GATEWAY_LLD_ENUMS_RETRYSTRATEGYTYPE_H

namespace payment_gateway_lld {
enum class RetryStrategyType { LINEAR, EXPONENTIAL_BACKOFF };
}

#endif
