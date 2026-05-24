#ifndef GOOGLE_DOCS_LLD_SERVICES_EDIT_SERVICE_H
#define GOOGLE_DOCS_LLD_SERVICES_EDIT_SERVICE_H

#include <stdexcept>
#include <string>

#include "../collaboration/EditNotificationHub.h"
#include "../enums/EditType.h"
#include "../models/EditOperation.h"
#include "DocumentService.h"
#include "SharingService.h"
#include "UndoRedoService.h"

namespace google_docs_lld {

class EditService {
public:
    EditService(DocumentService* documents, SharingService* sharing, UndoRedoService* undo,
                EditNotificationHub* hub)
        : documents_(documents), sharing_(sharing), undo_(undo), hub_(hub) {}

    EditOperation applyEdit(const std::string& documentId, const std::string& userId, EditType type,
                            int position, const std::string& text, int deleteLength,
                            int expectedRevision) {
        sharing_->requireEdit(documentId, userId);
        Document& doc = documents_->getDocument(documentId);

        if (expectedRevision != doc.revision) {
            throw std::runtime_error("stale revision: expected " + std::to_string(expectedRevision) +
                                     " but document is at " + std::to_string(doc.revision));
        }

        undo_->pushSnapshot(documentId);

        validateEdit(type, position, text, deleteLength, static_cast<int>(doc.content.size()));

        if (type == EditType::INSERT) {
            doc.content.insert(static_cast<std::size_t>(position), text);
        } else {
            doc.content.erase(static_cast<std::size_t>(position),
                              static_cast<std::size_t>(deleteLength));
        }
        ++doc.revision;

        const std::string opId = "op_" + std::to_string(++opCounter_);
        EditOperation op{opId,     documentId, userId, type, position,
                         text,     deleteLength, expectedRevision};

        hub_->broadcast(documentId, userId, op, doc.revision);
        return op;
    }

    static void validateEdit(EditType type, int position, const std::string& text, int deleteLength,
                             int contentLength) {
        if (position < 0 || position > contentLength) {
            throw std::invalid_argument("edit position out of bounds");
        }
        if (type == EditType::INSERT) {
            if (text.empty()) {
                throw std::invalid_argument("INSERT requires non-empty text");
            }
            return;
        }
        if (deleteLength <= 0 || position + deleteLength > contentLength) {
            throw std::invalid_argument("DELETE length invalid for position");
        }
    }

private:
    DocumentService* documents_;
    SharingService* sharing_;
    UndoRedoService* undo_;
    EditNotificationHub* hub_;
    int opCounter_{0};
};

}  // namespace google_docs_lld

#endif
