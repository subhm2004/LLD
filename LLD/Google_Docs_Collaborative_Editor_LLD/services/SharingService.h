#ifndef GOOGLE_DOCS_LLD_SERVICES_SHARING_SERVICE_H
#define GOOGLE_DOCS_LLD_SERVICES_SHARING_SERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/PermissionLevel.h"
#include "../models/CollaboratorAccess.h"
#include "../models/Document.h"
#include "DocumentService.h"

namespace google_docs_lld {

class SharingService {
public:
    explicit SharingService(DocumentService* documents) : documents_(documents) {}

    void share(const std::string& documentId, const std::string& userId, PermissionLevel permission) {
        const Document& doc = documents_->getDocument(documentId);
        if (doc.ownerId == userId) {
            throw std::invalid_argument("owner already has full access");
        }
        const std::string key = documentId + ":" + userId;
        access_[key] = CollaboratorAccess{documentId, userId, permission};
    }

    PermissionLevel resolvePermission(const std::string& documentId, const std::string& userId) const {
        const Document& doc = documents_->getDocument(documentId);
        if (doc.ownerId == userId) {
            return PermissionLevel::EDIT;
        }
        const std::string key = documentId + ":" + userId;
        auto it = access_.find(key);
        if (it == access_.end()) {
            throw std::runtime_error("user has no access to document");
        }
        return it->second.permission;
    }

    void requireEdit(const std::string& documentId, const std::string& userId) const {
        if (!canEdit(resolvePermission(documentId, userId))) {
            throw std::runtime_error("EDIT permission required");
        }
    }

    std::vector<CollaboratorAccess> listCollaborators(const std::string& documentId) const {
        std::vector<CollaboratorAccess> list;
        for (const auto& entry : access_) {
            if (entry.second.documentId == documentId) {
                list.push_back(entry.second);
            }
        }
        return list;
    }

private:
    DocumentService* documents_;
    std::unordered_map<std::string, CollaboratorAccess> access_;
};

}  // namespace google_docs_lld

#endif
