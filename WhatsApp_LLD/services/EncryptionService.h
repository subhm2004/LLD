#ifndef ENCRYPTION_SERVICE_H
#define ENCRYPTION_SERVICE_H

#include <string>

namespace whatsapp_lld {

class EncryptionService {
public:
    virtual ~EncryptionService() = default;
    virtual std::string encrypt(const std::string& plainText) const = 0;
};

class NoOpEncryptionService : public EncryptionService {
public:
    std::string encrypt(const std::string& plainText) const override {
        return plainText;
    }
};

}  // namespace whatsapp_lld

#endif
