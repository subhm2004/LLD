#ifndef GOOGLE_DOCS_LLD_CORE_COLLABORATIVE_EDITOR_SYSTEM_H
#define GOOGLE_DOCS_LLD_CORE_COLLABORATIVE_EDITOR_SYSTEM_H

#include <string>
#include <vector>

#include "../collaboration/EditNotificationHub.h"
#include "../collaboration/IEditListener.h"
#include "../enums/EditType.h"
#include "../enums/PermissionLevel.h"
#include "../models/CollaboratorAccess.h"
#include "../models/CursorPosition.h"
#include "../models/Document.h"
#include "../models/EditOperation.h"
#include "../models/User.h"
#include "../services/DocumentService.h"
#include "../services/EditService.h"
#include "../services/PresenceService.h"
#include "../services/SharingService.h"
#include "../services/UndoRedoService.h"
#include "../services/UserRegistryService.h"

namespace google_docs_lld {

// Facade — Google Docs–style collaborative editor API
class CollaborativeEditorSystem {
public:
    CollaborativeEditorSystem()
        : sharing_(&documents_),
          presence_(&users_, &documents_),
          undo_(&documents_),
          edits_(&documents_, &sharing_, &undo_, &hub_) {}

    std::string registerUser(const std::string& displayName, const std::string& email) {
        return users_.registerUser(displayName, email);
    }

    std::string createDocument(const std::string& ownerId, const std::string& title) {
        users_.getUser(ownerId);
        return documents_.createDocument(ownerId, title);
    }

    void shareDocument(const std::string& documentId, const std::string& userId,
                       PermissionLevel permission) {
        users_.getUser(userId);
        sharing_.share(documentId, userId, permission);
    }

    void joinSession(const std::string& documentId, const std::string& userId,
                     IEditListener* listener) {
        sharing_.resolvePermission(documentId, userId);
        hub_.subscribe(documentId, userId, listener);
    }

    void leaveSession(const std::string& documentId, const std::string& userId) {
        hub_.unsubscribe(documentId, userId);
    }

    EditOperation insertText(const std::string& documentId, const std::string& userId, int position,
                             const std::string& text, int expectedRevision) {
        return edits_.applyEdit(documentId, userId, EditType::INSERT, position, text, 0,
                                expectedRevision);
    }

    EditOperation deleteText(const std::string& documentId, const std::string& userId, int position,
                             int length, int expectedRevision) {
        return edits_.applyEdit(documentId, userId, EditType::DELETE, position, "", length,
                                expectedRevision);
    }

    bool undoLastEdit(const std::string& documentId, const std::string& userId) {
        sharing_.requireEdit(documentId, userId);
        return undo_.undo(documentId);
    }

    Document getDocument(const std::string& documentId, const std::string& userId) const {
        sharing_.resolvePermission(documentId, userId);
        return documents_.getDocument(documentId);
    }

    void updateCursor(const std::string& documentId, const std::string& userId, int index) {
        sharing_.resolvePermission(documentId, userId);
        presence_.updateCursor(documentId, userId, index);
    }

    std::vector<CursorPosition> listCursors(const std::string& documentId) const {
        return presence_.listCursors(documentId);
    }

    std::vector<std::string> activeSessionUsers(const std::string& documentId) const {
        return hub_.activeUserIds(documentId);
    }

    std::vector<CollaboratorAccess> listCollaborators(const std::string& documentId) const {
        return sharing_.listCollaborators(documentId);
    }

    const User& getUser(const std::string& userId) const { return users_.getUser(userId); }

private:
    UserRegistryService users_;
    DocumentService documents_;
    SharingService sharing_;
    PresenceService presence_;
    UndoRedoService undo_;
    EditNotificationHub hub_;
    EditService edits_;
};

}  // namespace google_docs_lld

#endif
