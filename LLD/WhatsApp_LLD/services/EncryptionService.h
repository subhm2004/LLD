#ifndef ENCRYPTION_SERVICE_H
#define ENCRYPTION_SERVICE_H

#include <bits/stdc++.h>
#include <string>

#include "crypto/ChatSessionCipher.h"

namespace whatsapp_lld {

class EncryptionService {
public:
  virtual ~EncryptionService() = default;

  // contextId = chatId for per-session key derivation
  virtual std::string encrypt(const std::string &plainText,
                              const std::string &contextId = "") const = 0;
  virtual std::string decrypt(const std::string &cipherText,
                              const std::string &contextId = "") const = 0;
};

class NoOpEncryptionService : public EncryptionService {
public:
  std::string encrypt(const std::string &plainText,
                      const std::string & /*contextId*/) const override {
    return plainText;
  }

  std::string decrypt(const std::string &cipherText,
                      const std::string & /*contextId*/) const override {
    return cipherText;
  }
};

// Per-chat session encryption (Strategy) — XOR + Base64, keyed by masterSecret
// + chatId
class ChatSessionEncryptionService : public EncryptionService {
public:
  explicit ChatSessionEncryptionService(std::string masterSecret)
      : masterSecret_(std::move(masterSecret)) {}

  std::string encrypt(const std::string &plainText,
                      const std::string &contextId) const override {
    const std::string key =
        crypto::ChatSessionCipher::deriveKey(masterSecret_, contextId);
    return crypto::ChatSessionCipher::encrypt(plainText, key);
  }

  std::string decrypt(const std::string &cipherText,
                      const std::string &contextId) const override {
    if (cipherText.empty() ||
        cipherText.rfind(crypto::ChatSessionCipher::kPrefix, 0) != 0) {
      return cipherText;
    }
    const std::string key =
        crypto::ChatSessionCipher::deriveKey(masterSecret_, contextId);
    return crypto::ChatSessionCipher::decrypt(cipherText, key);
  }

private:
  std::string masterSecret_;
};

} // namespace whatsapp_lld

#endif
