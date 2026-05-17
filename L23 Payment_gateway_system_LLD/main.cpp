#include <bits/stdc++.h>

#include "controllers/PaymentController.h"

using namespace std;
using namespace payment_gateway_lld;

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    PaymentRequest *req1 = new PaymentRequest("Aditya", "Shubham", 1000.0, "INR");
    cout << "Processing via Paytm\n";
    cout << "------------------------------\n";
    bool res1 = PaymentController::getInstance().handlePayment(GatewayType::PAYTM, req1);
    cout << "Result: " << (res1 ? "SUCCESS" : "FAIL") << "\n";
    cout << "------------------------------\n\n";

    PaymentRequest *req2 = new PaymentRequest("Shubham", "Aditya", 500.0, "USD");
    cout << "Processing via Razorpay\n";
    cout << "------------------------------\n";
    bool res2 = PaymentController::getInstance().handlePayment(GatewayType::RAZORPAY, req2);
    cout << "Result: " << (res2 ? "SUCCESS" : "FAIL") << "\n";
    cout << "------------------------------\n";

    delete req1;
    delete req2;
    return 0;
}
