#ifndef GOOGLE_DOCS_LLD_SERVICES_PRESENCE_SERVICE_H
#define GOOGLE_DOCS_LLD_SERVICES_PRESENCE_SERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/CursorPosition.h"
#include "DocumentService.h"
#include "UserRegistryService.h"

namespace google_docs_lld {

class PresenceService {
public:
    PresenceService(UserRegistryService* users, DocumentService* documents)
        : users_(users), documents_(documents) {}

    void updateCursor(const std::string& documentId, const std::string& userId, int index) {
        documents_->getDocument(documentId);
        const User& user = users_->getUser(userId);
        if (index < 0) {
            throw std::invalid_argument("cursor index cannot be negative");
        }
        cursors_[documentId][userId] = CursorPosition{userId, user.displayName, index};
    }

    std::vector<CursorPosition> listCursors(const std::string& documentId) const {
        std::vector<CursorPosition> list;
        auto docIt = cursors_.find(documentId);
        if (docIt == cursors_.end()) {
            return list;
        }
        for (const auto& entry : docIt->second) {
            list.push_back(entry.second);
        }
        return list;
    }

private:
    UserRegistryService* users_;
    DocumentService* documents_;
    std::unordered_map<std::string, std::unordered_map<std::string, CursorPosition>> cursors_;
};

}  // namespace google_docs_lld

#endif
