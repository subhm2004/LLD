#ifndef MEETING_SCHEDULER_LLD_MODELS_CALENDAR_H
#define MEETING_SCHEDULER_LLD_MODELS_CALENDAR_H

#include <string>
#include <vector>
#include <unordered_set>
#include "AvailabilityWindow.h"

namespace meeting_scheduler_lld {

/**
 * @class Calendar
 * @brief Har user ka ek personal calendar hota hai jo unki availability aur active meetings ko track karta hai.
 * 
 * Ye R6 requirement ko follow karta hai: "Every user should have a calendar that helps track dates and times".
 * Isme availability windows aur dynamic meetings (or invitations) ke references hote hain.
 */
class Calendar {
public:
    Calendar() = default;

    // Availability window add karne ke liye
    void addAvailability(const AvailabilityWindow& window) {
        availabilityWindows_.push_back(window);
    }

    // Kisi particular date ki saari availability list return karne ke liye
    std::vector<AvailabilityWindow> getAvailabilityForDate(const std::string& date) const {
        std::vector<AvailabilityWindow> result;
        for (const auto& window : availabilityWindows_) {
            if (window.getDate() == date) {
                result.push_back(window);
            }
        }
        return result;
    }

    // Sabhi availability windows pane ke liye
    const std::vector<AvailabilityWindow>& getAllAvailability() const {
        return availabilityWindows_;
    }

    // Calendar me meeting schedule hone par id register karne ke liye
    void addMeeting(const std::string& meetingId) {
        meetingIds_.insert(meetingId);
    }

    // Meeting cancel hone par calendar se hatane ke liye
    void removeMeeting(const std::string& meetingId) {
        meetingIds_.erase(meetingId);
    }

    // Check karne ke liye ki kya user is meeting me booked hai
    bool hasMeeting(const std::string& meetingId) const {
        return meetingIds_.count(meetingId) > 0;
    }

    // Getter for meeting IDs
    const std::unordered_set<std::string>& getMeetingIds() const {
        return meetingIds_;
    }

private:
    std::vector<AvailabilityWindow> availabilityWindows_; // User ki availability slots ki list
    std::unordered_set<std::string> meetingIds_;          // User ke mapped/scheduled meetings ki unique IDs
};

} // namespace meeting_scheduler_lld

#endif // MEETING_SCHEDULER_LLD_MODELS_CALENDAR_H
