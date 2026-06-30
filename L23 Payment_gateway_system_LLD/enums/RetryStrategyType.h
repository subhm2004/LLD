// enums/RetryStrategyType.h — Retry policy types: LINEAR (fixed wait) ya
// EXPONENTIAL_BACKOFF (badhta wait). RetryStrategyFactory isse choose karta hai.
#ifndef PAYMENT_GATEWAY_LLD_ENUMS_RETRYSTRATEGYTYPE_H
#define PAYMENT_GATEWAY_LLD_ENUMS_RETRYSTRATEGYTYPE_H

namespace payment_gateway_lld {
enum class RetryStrategyType { LINEAR, EXPONENTIAL_BACKOFF };
}

#endif
