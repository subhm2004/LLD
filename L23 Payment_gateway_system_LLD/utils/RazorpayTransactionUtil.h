#ifndef PAYMENT_GATEWAY_LLD_UTILS_RAZORPAYTRANSACTIONUTIL_H
#define PAYMENT_GATEWAY_LLD_UTILS_RAZORPAYTRANSACTIONUTIL_H

#include <bits/stdc++.h>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

namespace payment_gateway_lld {
class RazorpayTransactionUtil {
    static size_t nameHash(const string &name) {
        size_t hash = 0;
        for (char ch : name) {
            hash = hash * 31 + static_cast<unsigned char>(tolower(static_cast<unsigned char>(ch)));
        }
        return hash;
    }

public:
    static string toRazorpayAccount(const string &name) {
        size_t hash = nameHash(name);
        ostringstream oss;
        oss << "50100" << setw(7) << setfill('0') << (hash % 10000000);
        return oss.str();
    }

    static string formatCompletedAt() {
        auto now = chrono::system_clock::now();
        time_t t = chrono::system_clock::to_time_t(now);
        tm localTime{};
        localtime_r(&t, &localTime);
        ostringstream oss;
        oss << put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    static string generatePaymentId() {
        static int counter = 0;
        auto now = chrono::system_clock::now();
        time_t t = chrono::system_clock::to_time_t(now);
        tm localTime{};
        localtime_r(&t, &localTime);
        ostringstream oss;
        oss << "pay_" << put_time(&localTime, "%Y%m%d%H%M%S")
            << setw(4) << setfill('0') << (++counter % 10000);
        return oss.str();
    }
};
}

#endif
