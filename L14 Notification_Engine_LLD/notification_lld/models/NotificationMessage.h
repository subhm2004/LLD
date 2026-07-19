// ============================================================================
//  models/NotificationMessage.h — Ek notification ka data (id/title/body/priority)
// ----------------------------------------------------------------------------
//  Immutable data holder (sab getters, koi setter). NotificationSystem
//  isse banata hai, phir send() me iska title+body decorator chain me
//  jaata hai.
//
//  ⚠️ NOTE (interview-worthy detail): is file me neeche DUSRA #ifndef
//  block bhi hai — ek alternate NotificationMessage (bina title_ ke).
//  Par same include-guard (NOTIFICATION_MESSAGE_H) ki wajah se compiler
//  us dusre block ko SKIP kar deta hai — wo DEAD CODE hai, sirf upar wala
//  (title wala) active rehta hai. Isi liye code sahi chalta hai. Ideally
//  wo dead block hata dena chahiye taaki confusion na ho.
// ============================================================================
#ifndef NOTIFICATION_MESSAGE_H
#define NOTIFICATION_MESSAGE_H

#include <string>
#include <utility>
#include "../enums/NotificationPriority.h"

namespace notification_lld {

class NotificationMessage {
private:
    std::string id_;
    std::string title_;
    std::string body_;
    NotificationPriority priority_;

public:
    NotificationMessage(std::string id, std::string title, std::string body, NotificationPriority priority)
        : id_(std::move(id)),
          title_(std::move(title)),
          body_(std::move(body)),
          priority_(priority) {}

    const std::string& getId() const { return id_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getBody() const { return body_; }
    NotificationPriority getPriority() const { return priority_; }
};

}  // namespace notification_lld

#endif
#ifndef NOTIFICATION_MESSAGE_H
#define NOTIFICATION_MESSAGE_H

#include <string>
#include "../enums/NotificationPriority.h"

namespace notification_lld {

class NotificationMessage {
private:
    std::string id_;
    std::string body_;
    NotificationPriority priority_;

public:
    NotificationMessage(std::string id, std::string body, NotificationPriority priority)
        : id_(std::move(id)), body_(std::move(body)), priority_(priority) {}

    const std::string& getId() const { return id_; }
    const std::string& getBody() const { return body_; }
    NotificationPriority getPriority() const { return priority_; }
};

}  // namespace notification_lld

#endif
