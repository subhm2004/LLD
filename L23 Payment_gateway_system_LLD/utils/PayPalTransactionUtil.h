// utils/PayPalTransactionUtil.h — PayPal ke liye helper: unique transaction ID,
// timestamp etc. generate karta hai.
#ifndef PAYMENT_GATEWAY_LLD_UTILS_PAYPALTRANSACTIONUTIL_H
#define PAYMENT_GATEWAY_LLD_UTILS_PAYPALTRANSACTIONUTIL_H

#include <bits/stdc++.h>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

namespace payment_gateway_lld {
class PayPalTransactionUtil {
public:
    static string toPayPalEmail(const string &name) {
        string email = name;
        transform(email.begin(), email.end(), email.begin(),
                  [](unsigned char c) { return static_cast<char>(tolower(c)); });
        for (char &ch : email) {
            if (ch == ' ') ch = '.';
        }
        return email + "@paypal.com";
    }

    static string formatTimestamp() {
        auto now = chrono::system_clock::now();
        time_t t = chrono::system_clock::to_time_t(now);
        tm localTime{};
        localtime_r(&t, &localTime);
        ostringstream oss;
        oss << put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    static string generateTransactionId() {
        static int counter = 0;
        auto now = chrono::system_clock::now();
        time_t t = chrono::system_clock::to_time_t(now);
        tm localTime{};
        localtime_r(&t, &localTime);
        ostringstream oss;
        oss << "PAYPAL-" << put_time(&localTime, "%Y%m%d%H%M%S")
            << setw(4) << setfill('0') << (++counter % 10000);
        return oss.str();
    }
};
}

#endif
