#ifndef GOOGLE_DOCS_LLD_SERVICES_UNDO_REDO_SERVICE_H
#define GOOGLE_DOCS_LLD_SERVICES_UNDO_REDO_SERVICE_H

#include <deque>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../models/Document.h"
#include "DocumentService.h"

namespace google_docs_lld {

// Memento — per-document snapshot stack for undo
class UndoRedoService {
public:
    explicit UndoRedoService(DocumentService* documents) : documents_(documents) {}

    void pushSnapshot(const std::string& documentId) {
        const Document& doc = documents_->getDocument(documentId);
        stacks_[documentId].push_back({doc.content, doc.revision});
        if (stacks_[documentId].size() > kMaxDepth) {
            stacks_[documentId].pop_front();
        }
    }

    bool undo(const std::string& documentId) {
        auto it = stacks_.find(documentId);
        if (it == stacks_.end() || it->second.empty()) {
            return false;
        }
        const auto snapshot = it->second.back();
        it->second.pop_back();

        Document& doc = documents_->getDocument(documentId);
        doc.content = snapshot.content;
        doc.revision = snapshot.revision;
        return true;
    }

private:
    struct Snapshot {
        std::string content;
        int revision;
    };

    DocumentService* documents_;
    std::unordered_map<std::string, std::deque<Snapshot>> stacks_;
    static constexpr std::size_t kMaxDepth = 50;
};

}  // namespace google_docs_lld

#endif
