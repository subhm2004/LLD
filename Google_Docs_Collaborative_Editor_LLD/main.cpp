#include <iostream>
#include <string>

#include "collaboration/IEditListener.h"
#include "core/CollaborativeEditorSystem.h"
#include "enums/EditType.h"
#include "enums/PermissionLevel.h"

using namespace std;
using namespace google_docs_lld;

class ConsoleEditListener : public IEditListener {
public:
    explicit ConsoleEditListener(string label) : label_(std::move(label)) {}

    void onRemoteEdit(const EditOperation& operation, int newRevision) override {
        cout << "  [" << label_ << " received] " << editTypeToString(operation.type)
             << " by " << operation.userId << " @ pos " << operation.position;
        if (operation.type == EditType::INSERT) {
            cout << " text=\"" << operation.text << "\"";
        } else {
            cout << " len=" << operation.length;
        }
        cout << " → revision " << newRevision << "\n";
    }

private:
    string label_;
};

static void printDocument(const CollaborativeEditorSystem& system, const string& docId,
                          const string& viewerId) {
    const Document doc = system.getDocument(docId, viewerId);
    cout << "  Content: \"" << doc.content << "\" (revision " << doc.revision << ")\n";
}

int main() {
    cout << "====== Google Docs / Collaborative Editor LLD Demo ======\n\n";

    CollaborativeEditorSystem editor;

    const string aliceId = editor.registerUser("Alice", "alice@docs.com");
    const string bobId = editor.registerUser("Bob", "bob@docs.com");
    const string charlieId = editor.registerUser("Charlie", "charlie@docs.com");

    cout << "--- Create document ---\n";
    const string docId = editor.createDocument(aliceId, "Sprint Retro Notes");
    cout << "  " << docId << " created by Alice\n";

    cout << "\n--- Share: Bob=EDIT, Charlie=VIEW ---\n";
    editor.shareDocument(docId, bobId, PermissionLevel::EDIT);
    editor.shareDocument(docId, charlieId, PermissionLevel::VIEW);

    ConsoleEditListener aliceListener("Alice");
    ConsoleEditListener bobListener("Bob");
    editor.joinSession(docId, aliceId, &aliceListener);
    editor.joinSession(docId, bobId, &bobListener);

    cout << "\n--- Alice inserts \"Hello \" at revision 0 ---\n";
    editor.insertText(docId, aliceId, 0, "Hello ", 0);
    printDocument(editor, docId, aliceId);

    cout << "\n--- Bob inserts \"World!\" at revision 1 (sees Alice's edit via hub) ---\n";
    const Document afterAlice = editor.getDocument(docId, bobId);
    editor.insertText(docId, bobId, static_cast<int>(afterAlice.content.size()), "World!", 1);
    printDocument(editor, docId, bobId);

    cout << "\n--- Cursor presence ---\n";
    editor.updateCursor(docId, aliceId, 0);
    editor.updateCursor(docId, bobId, 11);
    for (const CursorPosition& cursor : editor.listCursors(docId)) {
        cout << "  " << cursor.displayName << " @ index " << cursor.index << "\n";
    }

    cout << "\n--- Stale revision rejected ---\n";
    try {
        editor.insertText(docId, aliceId, 0, "X", 0);
        cout << "  ERROR: stale revision should fail\n";
    } catch (const exception& ex) {
        cout << "  Rejected: " << ex.what() << "\n";
    }

    cout << "\n--- Charlie (VIEW) cannot edit ---\n";
    try {
        const Document doc = editor.getDocument(docId, charlieId);
        editor.insertText(docId, charlieId, 0, "hack", doc.revision);
        cout << "  ERROR: VIEW should not edit\n";
    } catch (const exception& ex) {
        cout << "  Rejected: " << ex.what() << "\n";
    }

    cout << "\n--- Alice undoes last snapshot ---\n";
    editor.undoLastEdit(docId, aliceId);
    printDocument(editor, docId, aliceId);

    cout << "\n--- Active session users ---\n";
    for (const string& uid : editor.activeSessionUsers(docId)) {
        cout << "  " << editor.getUser(uid).displayName << " (" << uid << ")\n";
    }

    editor.leaveSession(docId, aliceId);
    editor.leaveSession(docId, bobId);

    cout << "\nDemo complete.\n";
    return 0;
}
