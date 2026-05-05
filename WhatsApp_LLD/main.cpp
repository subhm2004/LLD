#include "core/WhatsAppSystem.h"
#include "models/Message.h"
#include "services/NotificationEngine.h"
#include <bits/stdc++.h>

using namespace std;
using namespace whatsapp_lld;

class DemoEncryptionService : public EncryptionService {
public:
  string encrypt(const string &plainText) const override { return "enc(" + plainText + ")"; }
};

int main() {
  WhatsAppSystem *system = new WhatsAppSystem();
  DemoEncryptionService *encryptionService = new DemoEncryptionService();
  system->configureEncryption(encryptionService);

  system->registerUser("u1", "Shubham", "+91-9999999991");
  system->registerUser("u2", "Rahul", "+91-9999999992");
  system->registerUser("u3", "Aman", "+91-9999999993");
  system->setUserOnline("u1");
  system->setUserOnline("u2");
  system->setUserOffline("u2");

  system->createDirectChat("c1", {"u1", "u2"});
  Message dm("m1", "u1", "Hi Rahul!", MessageType::IMAGE);
  system->sendDirectMessage("c1", dm, "u2");
  Message dm2("m2", "u2", "Document sent", MessageType::DOCUMENT);
  system->sendDirectMessage("c1", dm2, "u1");

  system->createGroup("g1", "Weekend Plan", "u1", {"u1", "u2", "u3"});
  Message gm("m3", "u1", "Let's meet on Saturday", MessageType::VIDEO);
  system->sendGroupMessage("g1", gm);

  const auto &directMessages = system->getDirectChat("c1").getMessages();
  cout << "u2 online: " << (system->isUserOnline("u2") ? "yes" : "no") << endl;
  cout << "u2 last seen epoch: " << system->getUserLastSeen("u2") << endl;
  cout << "Direct chat seq#1: " << directMessages[0].getSequenceNumber() << ", payload="
       << directMessages[0].getContent() << endl;
  cout << "Direct chat seq#2: " << directMessages[1].getSequenceNumber() << ", payload="
       << directMessages[1].getContent() << endl;

  NotificationObservable *observable = NotificationHub::getInstance().getObservable();
  LoggerObserver logger(observable);
  NotificationEngine engine(observable);
  engine.addNotificationStrategy(make_unique<EmailStrategy>("demo@whatsapp.com"));
  engine.addNotificationStrategy(make_unique<SMSStrategy>("+91-9000000000"));
  engine.addNotificationStrategy(make_unique<PopUpStrategy>());

  shared_ptr<INotification> decorated = make_shared<SimpleNotification>("Group updated successfully");
  decorated = make_shared<TimestampDecorator>(decorated);
  decorated = make_shared<SignatureDecorator>(decorated, "WhatsAppSystem");
  NotificationHub::getInstance().sendNotification(decorated);

  delete system;
  return 0;
}
