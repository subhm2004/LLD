#ifndef PAYMENT_GATEWAY_LLD_UTILS_PAYTMTRANSACTIONUTIL_H
#define PAYMENT_GATEWAY_LLD_UTILS_PAYTMTRANSACTIONUTIL_H

#include <bits/stdc++.h>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

namespace payment_gateway_lld {
class PaytmTransactionUtil {
public:
    static string toPaytmUpi(const string &name) {
        string upi = name;
        transform(upi.begin(), upi.end(), upi.begin(),
                  [](unsigned char c) { return static_cast<char>(tolower(c)); });
        for (char &ch : upi) {
            if (ch == ' ') ch = '.';
        }
        return upi + "@paytm";
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

    static string generateReferenceNumber() {
        static int counter = 0;
        auto now = chrono::system_clock::now();
        time_t t = chrono::system_clock::to_time_t(now);
        tm localTime{};
        localtime_r(&t, &localTime);
        ostringstream oss;
        oss << "PAYTM" << put_time(&localTime, "%Y%m%d%H%M%S")
            << setw(4) << setfill('0') << (++counter % 10000);
        return oss.str();
    }
};
}

#endif
