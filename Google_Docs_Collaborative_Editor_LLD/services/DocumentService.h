#ifndef GOOGLE_DOCS_LLD_SERVICES_DOCUMENT_SERVICE_H
#define GOOGLE_DOCS_LLD_SERVICES_DOCUMENT_SERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/Document.h"

namespace google_docs_lld {

class DocumentService {
public:
    std::string createDocument(const std::string& ownerId, const std::string& title) {
        const std::string documentId = "doc_" + std::to_string(++counter_);
        documents_[documentId] = Document{documentId, title, ownerId, "", 0};
        return documentId;
    }

    Document& getDocument(const std::string& documentId) {
        auto it = documents_.find(documentId);
        if (it == documents_.end()) {
            throw std::invalid_argument("unknown document: " + documentId);
        }
        return it->second;
    }

    const Document& getDocument(const std::string& documentId) const {
        auto it = documents_.find(documentId);
        if (it == documents_.end()) {
            throw std::invalid_argument("unknown document: " + documentId);
        }
        return it->second;
    }

    std::vector<std::string> listByOwner(const std::string& ownerId) const {
        std::vector<std::string> ids;
        for (const auto& entry : documents_) {
            if (entry.second.ownerId == ownerId) {
                ids.push_back(entry.first);
            }
        }
        return ids;
    }

private:
    std::unordered_map<std::string, Document> documents_;
    int counter_{0};
};

}  // namespace google_docs_lld

#endif
