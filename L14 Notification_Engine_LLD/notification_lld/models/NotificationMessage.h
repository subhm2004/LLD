// models/NotificationMessage.h — Ek notification ka data: id, title, body, priority.
// NOTE: neeche dusra #ifndef block bhi hai (ek alternate definition bina title ke),
// par same include-guard ki wajah se woh DEAD hai — sirf upar wala active rehta hai.
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
