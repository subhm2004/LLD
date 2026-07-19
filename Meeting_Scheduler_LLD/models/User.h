#ifndef MEETING_SCHEDULER_LLD_MODELS_USER_H
#define MEETING_SCHEDULER_LLD_MODELS_USER_H

#include <string>
#include "Calendar.h"

namespace meeting_scheduler_lld {

/**
 * @class User
 * @brief System ke ek registered user ko represent karti hai.
 * 
 * Har user ka apna ek unique ID, name, email aur preferred timezone hota hai.
 * R6 requirement ke mutabik, har user ke paas ek personal `Calendar` instance bhi hota hai,
 * jo unki availability windows aur scheduled meetings ko maintain karta hai.
 */
class User {
public:
    User() = default;

    // Parameterized constructor jo user details initialize karta hai.
    User(std::string userId, std::string name, std::string email, std::string timezone)
        : userId_(std::move(userId)),
          name_(std::move(name)),
          email_(std::move(email)),
          timezone_(std::move(timezone)) {}

    // User ki unique ID get karne ke liye.
    const std::string& getUserId() const { return userId_; }
    
    // User ka naam pane ke liye.
    const std::string& getName() const { return name_; }
    
    // User ka email ID get karne ke liye (registration verification ke kaam aata hai).
    const std::string& getEmail() const { return email_; }
    
    // User ka time zone (e.g., "Asia/Kolkata") get karne ke liye.
    const std::string& getTimezone() const { return timezone_; }

    // User ke calendar ka reference read-only format me pane ke liye.
    const Calendar& getCalendar() const { return calendar_; }

    // User ke calendar ka mutable (modifiable) reference pane ke liye, taaki usme changes kiye ja sakein.
    Calendar& getCalendar() { return calendar_; }

private:
    std::string userId_;     // Unique User ID (e.g., "MS_U1")
    std::string name_;       // User ka full name
    std::string email_;      // Unique email address
    std::string timezone_;   // Timezone (standard calculations ke liye)
    Calendar calendar_;      // Personal calendar instance (R6 Requirement mapping)
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_MODELS_USER_H
