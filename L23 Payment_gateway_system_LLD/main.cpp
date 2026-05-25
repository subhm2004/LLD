#include <bits/stdc++.h>

#include "controllers/PaymentController.h"
#include "enums/BillingInterval.h"
#include "enums/RetryStrategyType.h"

using namespace std;
using namespace payment_gateway_lld;

int main() {
  srand(static_cast<unsigned>(time(nullptr)));

  PaymentRequest *req1 = new PaymentRequest("Hardik", "Shubham", 1000.0, "INR");
  cout << "Processing via Paytm\n";
  cout << "------------------------------\n";
  bool res1 =
      PaymentController::getInstance().handlePayment(GatewayType::PAYTM, req1);
  cout << "Result: " << (res1 ? "SUCCESS" : "FAIL") << "\n";
  cout << "------------------------------\n\n";

  PaymentRequest *req2 = new PaymentRequest("Shubham", "Hardik", 500.0, "USD");
  cout << "Processing via Razorpay\n";
  cout << "------------------------------\n";
  bool res2 = PaymentController::getInstance().handlePayment(
      GatewayType::RAZORPAY, req2);
  cout << "Result: " << (res2 ? "SUCCESS" : "FAIL") << "\n";
  cout << "------------------------------\n\n";

  PaymentRequest *reqPayPal = new PaymentRequest("Hardik", "Shubham", 75.0, "USD");
  cout << "Processing via PayPal\n";
  cout << "------------------------------\n";
  bool resPayPal = PaymentController::getInstance().handlePayment(GatewayType::PAYPAL, reqPayPal);
  cout << "Result: " << (resPayPal ? "SUCCESS" : "FAIL") << "\n";
  cout << "------------------------------\n\n";

  PaymentRequest *req3 = new PaymentRequest("Hardik", "Shubham", 1000.0, "INR");
  cout << "Processing via Paytm (Linear retry)\n";
  cout << "------------------------------\n";
  bool res3 = PaymentController::getInstance().handlePayment(
      GatewayType::PAYTM, req3, RetryStrategyType::LINEAR);
  cout << "Result: " << (res3 ? "SUCCESS" : "FAIL") << "\n";
  cout << "------------------------------\n\n";

  PaymentRequest *req4 = new PaymentRequest("Shubham", "Hardik", 500.0, "USD");
  cout << "Processing via Razorpay (Exponential back-off)\n";
  cout << "------------------------------\n";
  bool res4 = PaymentController::getInstance().handlePayment(
      GatewayType::RAZORPAY, req4, RetryStrategyType::EXPONENTIAL_BACKOFF);
  cout << "Result: " << (res4 ? "SUCCESS" : "FAIL") << "\n";
  cout << "------------------------------\n\n";

  cout << "Recurring payments / subscriptions\n";
  cout << "------------------------------\n";
  string subId = PaymentController::getInstance().createSubscription(
      "Hardik", "Netflix", 499.0, "INR", GatewayType::PAYTM,
      BillingInterval::MONTHLY);
  PaymentController::getInstance().createSubscription(
      "Shubham", "Spotify", 119.0, "INR", GatewayType::PAYTM,
      BillingInterval::WEEKLY);
  PaymentController::getInstance().processRecurringBilling(
      RetryStrategyType::LINEAR);
  PaymentController::getInstance().cancelSubscription(subId);
  PaymentController::getInstance().processRecurringBilling(
      RetryStrategyType::LINEAR);
  cout << "------------------------------\n";

  delete req1;
  delete req2;
  delete reqPayPal;
  delete req3;
  delete req4;
  return 0;
}
