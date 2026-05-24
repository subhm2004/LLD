#ifndef WHATSAPP_LLD_CRYPTO_BASE64_CODEC_H
#define WHATSAPP_LLD_CRYPTO_BASE64_CODEC_H

#include <string>
#include <vector>

namespace whatsapp_lld::crypto {

class Base64Codec {
public:
    static std::string encode(const std::string& input) {
        static const char kTable[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string output;
        int val = 0;
        int bits = -6;

        for (unsigned char c : input) {
            val = (val << 8) + c;
            bits += 8;
            while (bits >= 0) {
                output.push_back(kTable[(val >> bits) & 0x3F]);
                bits -= 6;
            }
        }

        if (bits > -6) {
            output.push_back(kTable[((val << 8) >> (bits + 8)) & 0x3F]);
        }

        while (output.size() % 4 != 0) {
            output.push_back('=');
        }
        return output;
    }

    static std::string decode(const std::string& input) {
        std::vector<unsigned char> bytes;
        int val = 0;
        int bits = -8;

        for (unsigned char c : input) {
            if (c == '=') {
                break;
            }
            const int decoded = decodeChar(static_cast<char>(c));
            if (decoded < 0) {
                continue;
            }
            val = (val << 6) + decoded;
            bits += 6;
            if (bits >= 0) {
                bytes.push_back(static_cast<unsigned char>((val >> bits) & 0xFF));
                bits -= 8;
            }
        }

        return std::string(bytes.begin(), bytes.end());
    }

private:
    static int decodeChar(char c) {
        if (c >= 'A' && c <= 'Z') {
            return c - 'A';
        }
        if (c >= 'a' && c <= 'z') {
            return c - 'a' + 26;
        }
        if (c >= '0' && c <= '9') {
            return c - '0' + 52;
        }
        if (c == '+') {
            return 62;
        }
        if (c == '/') {
            return 63;
        }
        return -1;
    }
};

}  // namespace whatsapp_lld::crypto

#endif
