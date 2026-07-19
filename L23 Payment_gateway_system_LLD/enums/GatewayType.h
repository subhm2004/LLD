// ============================================================================
//  enums/GatewayType.h — Provider types: PAYTM / RAZORPAY / PAYPAL
// ----------------------------------------------------------------------------
//  GatewayFactory isse decide karta kaunsa gateway banana. `enum class`
//  (type-safe). Naya provider (jaise GPAY) = ek value + naya gateway class +
//  factory me case — baaki system untouched (Factory + Template Method fayda).
// ============================================================================
#ifndef PAYMENT_GATEWAY_LLD_ENUMS_GATEWAYTYPE_H
#define PAYMENT_GATEWAY_LLD_ENUMS_GATEWAYTYPE_H

#include <bits/stdc++.h>
using namespace std;

namespace payment_gateway_lld {
enum class GatewayType { PAYTM, RAZORPAY, PAYPAL };
}

#endif
