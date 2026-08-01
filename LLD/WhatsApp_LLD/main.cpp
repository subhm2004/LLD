#include <bits/stdc++.h>

#include "core/WhatsAppSystem.h"
#include "enums/DeletionType.h"
#include "models/Message.h"
#include "services/EncryptionService.h"
#include "services/NotificationEngine.h"

using namespace std;
using namespace whatsapp_lld;

static void printTimeline(const string& label, const vector<Message>& messages,
                          const string& viewerId) {
    cout << label << " (viewer=" << viewerId << "):\n";
    for (const Message& message : messages) {
        const string display = message.getDisplayContent(viewerId);
        if (display.empty()) {
            continue;
        }
        cout << "  [" << message.getMessageId() << "] " << message.getSenderId() << ": " << display;
        if (message.isDeletedForEveryone()) {
            cout << "  (deleted for everyone)";
        }
        cout << "\n";
    }
}

static void printRawStore(const Chat& chat) {
    cout << "  Raw DB (encrypted at rest):\n";
    for (const Message& message : chat.getMessages()) {
        cout << "    " << message.getMessageId() << " -> \"" << message.getContent() << "\"\n";
    }
}

int main() {
    cout << "====== WhatsApp LLD Demo ======\n\n";

    WhatsAppSystem system;
    ChatSessionEncryptionService encryptionService("whatsapp-master-secret-demo");
    system.configureEncryption(&encryptionService);

    system.registerUser("u1", "Shubham", "+91-9999999991");
    system.registerUser("u2", "Rahul", "+91-9999999992");
    system.registerUser("u3", "Aman", "+91-9999999993");
    system.setUserOnline("u1");
    system.setUserOffline("u2");

    system.createDirectChat("c1", {"u1", "u2"});
    Message dm1("m1", "u1", "Hi Rahul!", MessageType::TEXT);
    system.sendDirectMessage("c1", dm1, "u2");
    Message dm2("m2", "u2", "Hey Shubham, document attached", MessageType::DOCUMENT);
    system.sendDirectMessage("c1", dm2, "u1");

    cout << "--- Encryption: at-rest vs decrypted view ---\n";
    printRawStore(system.getDirectChat("c1"));
    printTimeline("Decrypted for u1", system.getDirectMessagesForUser("c1", "u1"), "u1");

    cout << "\n--- Message deletion ---\n";
    system.deleteDirectMessage("c1", "m1", "u2", DeletionType::DELETE_FOR_ME);
    printTimeline("u2 after delete-for-me", system.getDirectMessagesForUser("c1", "u2"), "u2");

    const time_t now = time(nullptr);
    system.deleteDirectMessage("c1", "m2", "u2", DeletionType::DELETE_FOR_EVERYONE, now);
    printTimeline("Both after delete-for-everyone on m2", system.getDirectMessagesForUser("c1", "u1"),
                  "u1");

    cout << "\n--- Per-chat session key (wrong chatId = garbage) ---\n";
    const string cipher = system.getDirectChat("c1").getMessages()[0].getContent();
    const string correct = encryptionService.decrypt(cipher, "c1");
    const string wrongKey = encryptionService.decrypt(cipher, "c99");
    cout << "  Correct key: \"" << correct << "\"\n";
    cout << "  Wrong chatId:  \"" << wrongKey << "\" (unreadable — different session key)\n";

    cout << "\n--- Group + encryption ---\n";
    system.createGroup("g1", "Weekend Plan", "u1", {"u1", "u2", "u3"});
    Message gm("m3", "u1", "Let's meet on Saturday", MessageType::TEXT);
    system.sendGroupMessage("g1", gm);
    printRawStore(system.getGroup("g1"));
    printTimeline("g1 decrypted", system.getGroupMessagesForUser("g1", "u1"), "u1");

    cout << "\nu2 online: " << (system.isUserOnline("u2") ? "yes" : "no") << "\n";

    NotificationObservable* observable = NotificationHub::getInstance().getObservable();
    LoggerObserver logger(observable);
    NotificationEngine engine(observable);
    engine.addNotificationStrategy(make_unique<EmailStrategy>("demo@whatsapp.com"));
    engine.addNotificationStrategy(make_unique<SMSStrategy>("+91-9000000000"));
    engine.addNotificationStrategy(make_unique<PopUpStrategy>());

    shared_ptr<INotification> decorated =
        make_shared<SimpleNotification>("Chat session encryption validated");
    decorated = make_shared<TimestampDecorator>(decorated);
    decorated = make_shared<SignatureDecorator>(decorated, "WhatsAppSystem");
    NotificationHub::getInstance().sendNotification(decorated);

    cout << "\nDemo complete.\n";
    return 0;
}
