#ifndef TRUECALLER_LLD_MODELS_CONTACTENTRY_H
#define TRUECALLER_LLD_MODELS_CONTACTENTRY_H

#include <string>

#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

/** User's synced contact (local name + phone). */
class ContactEntry {
public:
    ContactEntry() = default;

    ContactEntry(std::string localName, std::string phone)
        : localName_(std::move(localName)), phone_(normalizePhone(std::move(phone))) {}

    const std::string& getLocalName() const { return localName_; }
    const std::string& getPhone() const { return phone_; }

    void setDirectoryName(const std::string& name) { directoryName_ = name; }
    const std::string& getDirectoryName() const { return directoryName_; }
    bool hasDirectoryMatch() const { return !directoryName_.empty(); }

private:
    std::string localName_;
    std::string phone_;
    std::string directoryName_;
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_MODELS_CONTACTENTRY_H
