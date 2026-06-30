// enums/GatewayType.h — Provider types: PAYTM / RAZORPAY / PAYPAL.
// GatewayFactory isse decide karta hai kaunsa gateway banana hai.
#ifndef PAYMENT_GATEWAY_LLD_ENUMS_GATEWAYTYPE_H
#define PAYMENT_GATEWAY_LLD_ENUMS_GATEWAYTYPE_H

#include <bits/stdc++.h>
using namespace std;

namespace payment_gateway_lld {
enum class GatewayType { PAYTM, RAZORPAY, PAYPAL };
}

#endif
