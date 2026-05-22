#ifndef MEETING_SCHEDULER_LLD_MODELS_USER_H
#define MEETING_SCHEDULER_LLD_MODELS_USER_H

#include <string>

namespace meeting_scheduler_lld {

class User {
public:
    User() = default;

    User(std::string userId, std::string name, std::string email, std::string timezone)
        : userId_(std::move(userId)),
          name_(std::move(name)),
          email_(std::move(email)),
          timezone_(std::move(timezone)) {}

    const std::string& getUserId() const { return userId_; }
    const std::string& getName() const { return name_; }
    const std::string& getEmail() const { return email_; }
    const std::string& getTimezone() const { return timezone_; }

private:
    std::string userId_;
    std::string name_;
    std::string email_;
    std::string timezone_;
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_MODELS_USER_H
