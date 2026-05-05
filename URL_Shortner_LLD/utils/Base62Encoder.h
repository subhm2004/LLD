#ifndef URL_SHORTNER_LLD_UTILS_BASE62ENCODER_H
#define URL_SHORTNER_LLD_UTILS_BASE62ENCODER_H

#include <bits/stdc++.h>

using namespace std;

namespace url_shortner_lld {

class Base62Encoder {
public:
    static string encode(long long value) {
        const string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        if (value == 0) {
            return "0";
        }

        string encoded;
        while (value > 0) {
            encoded.push_back(chars[value % 62]);
            value /= 62;
        }
        reverse(encoded.begin(), encoded.end());
        return encoded;
    }
};

} // namespace url_shortner_lld

#endif // URL_SHORTNER_LLD_UTILS_BASE62ENCODER_H
