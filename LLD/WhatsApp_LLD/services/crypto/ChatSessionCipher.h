#ifndef WHATSAPP_LLD_CRYPTO_CHAT_SESSION_CIPHER_H
#define WHATSAPP_LLD_CRYPTO_CHAT_SESSION_CIPHER_H

#include <cstdint>
#include <stdexcept>
#include <string>

#include "Base64Codec.h"

namespace whatsapp_lld::crypto {

// Stream-style XOR cipher with per-chat derived key (LLD demo — not production crypto)
class ChatSessionCipher {
public:
    static constexpr const char* kPrefix = "WA1:";

    static std::string deriveKey(const std::string& masterSecret, const std::string& chatId) {
        std::uint64_t hash = 1469598103934665603ULL;
        for (char c : masterSecret) {
            hash ^= static_cast<unsigned char>(c);
            hash *= 1099511628211ULL;
        }
        for (char c : chatId) {
            hash ^= static_cast<unsigned char>(c);
            hash *= 1099511628211ULL;
        }
        std::string key;
        for (int i = 0; i < 16; ++i) {
            hash = hash * 6364136223846793005ULL + 1;
            key.push_back(static_cast<char>((hash >> 33) & 0xFF));
        }
        return key;
    }

    static std::string encrypt(const std::string& plainText, const std::string& key) {
        if (plainText.empty()) {
            return std::string(kPrefix);
        }
        std::string xored;
        xored.reserve(plainText.size());
        for (std::size_t i = 0; i < plainText.size(); ++i) {
            const unsigned char p = static_cast<unsigned char>(plainText[i]);
            const unsigned char k = static_cast<unsigned char>(key[i % key.size()]);
            xored.push_back(static_cast<char>(p ^ k));
        }
        return std::string(kPrefix) + Base64Codec::encode(xored);
    }

    static std::string decrypt(const std::string& cipherText, const std::string& key) {
        if (cipherText.rfind(kPrefix, 0) != 0) {
            throw std::invalid_argument("unsupported ciphertext format");
        }
        const std::string payload = cipherText.substr(std::string(kPrefix).size());
        if (payload.empty()) {
            return "";
        }
        const std::string xored = Base64Codec::decode(payload);
        std::string plain;
        plain.reserve(xored.size());
        for (std::size_t i = 0; i < xored.size(); ++i) {
            const unsigned char x = static_cast<unsigned char>(xored[i]);
            const unsigned char k = static_cast<unsigned char>(key[i % key.size()]);
            plain.push_back(static_cast<char>(x ^ k));
        }
        return plain;
    }
};

}  // namespace whatsapp_lld::crypto

#endif
