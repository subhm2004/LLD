#ifndef GOOGLE_DOCS_LLD_COLLABORATION_EDIT_NOTIFICATION_HUB_H
#define GOOGLE_DOCS_LLD_COLLABORATION_EDIT_NOTIFICATION_HUB_H

#include <string>
#include <unordered_map>
#include <vector>

#include "IEditListener.h"

namespace google_docs_lld {

// Observer hub — broadcasts applied edits to active session listeners
class EditNotificationHub {
public:
    void subscribe(const std::string& documentId, const std::string& userId, IEditListener* listener) {
        listeners_[documentId][userId] = listener;
    }

    void unsubscribe(const std::string& documentId, const std::string& userId) {
        auto docIt = listeners_.find(documentId);
        if (docIt == listeners_.end()) {
            return;
        }
        docIt->second.erase(userId);
    }

    void broadcast(const std::string& documentId, const std::string& sourceUserId,
                   const EditOperation& operation, int newRevision) {
        auto docIt = listeners_.find(documentId);
        if (docIt == listeners_.end()) {
            return;
        }
        for (const auto& entry : docIt->second) {
            if (entry.first == sourceUserId || entry.second == nullptr) {
                continue;
            }
            entry.second->onRemoteEdit(operation, newRevision);
        }
    }

    std::vector<std::string> activeUserIds(const std::string& documentId) const {
        std::vector<std::string> ids;
        auto docIt = listeners_.find(documentId);
        if (docIt == listeners_.end()) {
            return ids;
        }
        for (const auto& entry : docIt->second) {
            ids.push_back(entry.first);
        }
        return ids;
    }

private:
    std::unordered_map<std::string, std::unordered_map<std::string, IEditListener*>> listeners_;
};

}  // namespace google_docs_lld

#endif
