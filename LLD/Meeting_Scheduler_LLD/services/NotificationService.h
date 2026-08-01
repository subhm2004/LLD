#ifndef MEETING_SCHEDULER_LLD_SERVICES_NOTIFICATIONSERVICE_H
#define MEETING_SCHEDULER_LLD_SERVICES_NOTIFICATIONSERVICE_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

#include "../models/Meeting.h"

namespace meeting_scheduler_lld {

/**
 * @interface INotificationObserver
 * @brief Observer Pattern ka interface jo define karta hai ki notifications kaise receive honge.
 * 
 * R4 requirement: "A notification must be sent to everyone invited to the meeting".
 * Is interface ko implement karne wale classes notifications listen karenge.
 */
class INotificationObserver {
public:
    virtual ~INotificationObserver() = default;

    // Meeting book hone par notify karne ke liye method.
    virtual void onMeetingScheduled(const Meeting& meeting) = 0;

    // Meeting cancel hone par notify karne ke liye method.
    virtual void onMeetingCancelled(const Meeting& meeting, const std::string& requesterId) = 0;

    // Kisi invitee ke response (accept/decline) par notify karne ke liye method.
    virtual void onInvitationUpdated(const Meeting& meeting, const std::string& userId, InvitationStatus newStatus) = 0;
};

/**
 * @class ConsoleNotificationObserver
 * @brief Console implementation jo system updates ko screen par notify/print karta hai.
 */
class ConsoleNotificationObserver : public INotificationObserver {
public:
    void onMeetingScheduled(const Meeting& meeting) override {
        std::cout << "[NOTIFICATION] Nayi Meeting Schedule hui: '" << meeting.getTitle()
                  << "' (ID: " << meeting.getMeetingId() << ") Room ID: " << meeting.getRoomId()
                  << " me on Date: " << meeting.getDate() << " (" << meeting.getStartMinutes()
                  << " to " << meeting.getEndMinutes() << " minutes)\n";
    }

    void onMeetingCancelled(const Meeting& meeting, const std::string& requesterId) override {
        std::cout << "[NOTIFICATION] Meeting Cancel kar di gayi hai (Requester: " << requesterId 
                  << "): '" << meeting.getTitle() << "' (ID: " << meeting.getMeetingId() << ")\n";
    }

    void onInvitationUpdated(const Meeting& meeting, const std::string& userId, InvitationStatus newStatus) override {
        std::string statusStr = "PENDING";
        if (newStatus == InvitationStatus::ACCEPTED) statusStr = "ACCEPTED";
        else if (newStatus == InvitationStatus::DECLINED) statusStr = "DECLINED";

        std::cout << "[NOTIFICATION] User " << userId << " ne Meeting '" << meeting.getTitle()
                  << "' (ID: " << meeting.getMeetingId() << ") ki invitation par apna response " 
                  << statusStr << " bheja hai.\n";
    }
};

/**
 * @class NotificationService
 * @brief Notification system ka subject class jo observers/listeners ko notify karta hai.
 * 
 * Isme registerObserver aur status wise notification methods provide kiye gaye hain.
 */
class NotificationService {
public:
    // Observer list me add karne ke liye
    void registerObserver(std::shared_ptr<INotificationObserver> observer) {
        if (observer) {
            observers_.push_back(observer);
        }
    }

    // Sabhi observers ko schedule event send karne ke liye
    void notifyScheduled(const Meeting& meeting) {
        for (const auto& observer : observers_) {
            observer->onMeetingScheduled(meeting);
        }
    }

    // Sabhi observers ko cancel event send karne ke liye
    void notifyCancelled(const Meeting& meeting, const std::string& requesterId) {
        for (const auto& observer : observers_) {
            observer->onMeetingCancelled(meeting, requesterId);
        }
    }

    // Sabhi observers ko status change notification bhejne ke liye
    void notifyInvitationUpdated(const Meeting& meeting, const std::string& userId, InvitationStatus status) {
        for (const auto& observer : observers_) {
            observer->onInvitationUpdated(meeting, userId, status);
        }
    }

private:
    std::vector<std::shared_ptr<INotificationObserver>> observers_; // Register kiye gaye listeners ki list
};

} // namespace meeting_scheduler_lld

#endif // MEETING_SCHEDULER_LLD_SERVICES_NOTIFICATIONSERVICE_H
