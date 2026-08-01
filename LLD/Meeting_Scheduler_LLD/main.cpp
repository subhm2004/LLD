#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>

#include "core/MeetingSchedulerSystem.h"
#include "enums/MeetingStatus.h"
#include "enums/InvitationStatus.h"
#include "services/NotificationService.h"

using namespace std;
using namespace meeting_scheduler_lld;

/**
 * @file main.cpp
 * @brief Meeting Scheduler Application ka main runner executable program.
 * 
 * Hinglish comments:
 * - Is main function me humne meeting rooms, notifications (Observer pattern), calendars,
 *   aur flexible invitation flows ke tests aur cases add kiye hain taaki saare conditions verify ho sakein.
 */

// Time minutes format helper function (e.g. 540 -> "09:00")
static string formatMinutes(int minutes) {
    int h = minutes / 60;
    int m = minutes % 60;
    ostringstream oss;
    oss << setfill('0') << setw(2) << h << ":" << setw(2) << m;
    return oss.str();
}

// MeetingStatus enum ko string description me badalne ke liye function.
static string statusToString(MeetingStatus status) {
    return status == MeetingStatus::SCHEDULED ? "SCHEDULED" : "CANCELLED";
}

// InvitationStatus enum ko readable string representation me translate karne ke liye helper.
static string inviteStatusToString(InvitationStatus status) {
    switch (status) {
        case InvitationStatus::PENDING: return "PENDING";
        case InvitationStatus::ACCEPTED: return "ACCEPTED";
        case InvitationStatus::DECLINED: return "DECLINED";
    }
    return "UNKNOWN";
}

int main() {
    cout << "====== Meeting Scheduler LLD Enhanced Demo ======\n\n";

    MeetingSchedulerSystem scheduler;

    // 1. Notification Observer register karna (R4: Observer Design Pattern)
    // ConsoleNotificationObserver ko add karenge taaki notification print outputs dikhein.
    auto consoleObserver = make_shared<ConsoleNotificationObserver>();
    scheduler.registerNotificationObserver(consoleObserver);

    // 2. Meeting Rooms set up karna (R1, R2 Requirements)
    cout << "\n--- Adding Meeting Rooms ---\n";
    scheduler.addMeetingRoom("ROOM_A", "Grand Ballroom", 10);
    scheduler.addMeetingRoom("ROOM_B", "Mini Cabin", 2); // capacity is only 2 people
    cout << "  Room ROOM_A (Capacity: 10) aur ROOM_B (Capacity: 2) add ho chuke hain.\n";

    const string date = "2026-05-25";

    // 3. Register Users (FR1)
    const string aliceId = scheduler.registerUser("Alice", "alice@work.com", "Asia/Kolkata");
    const string bobId = scheduler.registerUser("Bob", "bob@work.com", "Asia/Kolkata");
    const string carolId = scheduler.registerUser("Carol", "carol@work.com", "Asia/Kolkata");

    // 4. Set Availability Windows (FR2, R6)
    scheduler.setAvailability(aliceId, date, 9 * 60, 12 * 60);  // 9:00 to 12:00
    scheduler.setAvailability(aliceId, date, 14 * 60, 17 * 60); // 14:00 to 17:00
    scheduler.setAvailability(bobId, date, 10 * 60, 16 * 60);   // 10:00 to 16:00
    scheduler.setAvailability(carolId, date, 11 * 60, 15 * 60); // 11:00 to 15:00

    // 5. Find Mutual Free Slots (FR8)
    cout << "\n--- Mutual free slots (60 min) ---\n";
    vector<TimeSlot> slots = scheduler.findMutualFreeSlots({aliceId, bobId, carolId}, date, 60);
    for (const TimeSlot& slot : slots) {
        cout << "  " << slot.getDate() << " " << formatMinutes(slot.getStartMinutes()) << "-"
             << formatMinutes(slot.getEndMinutes()) << "\n";
    }

    // 6. Schedule Meeting 1:1 with specific room and check capacity rules (R2, R3)
    cout << "\n--- Schedule 1:1 (Alice + Bob) in ROOM_B (Capacity limit matching) ---\n";
    Meeting m1 = scheduler.scheduleMeeting(aliceId, {bobId}, "Sprint planning", date,
                                           10 * 60, 11 * 60, "ROOM_B");
    cout << "  " << m1.getMeetingId() << " " << m1.getTitle() << " reserved "
         << formatMinutes(m1.getStartMinutes()) << "-" << formatMinutes(m1.getEndMinutes())
         << " in room: " << m1.getRoomId() << " (" << statusToString(m1.getStatus()) << ")\n";

    // 7. Verify Capacity check failure (ROOM_B capacity is 2, but we try to book for 3 participants)
    cout << "\n--- Room Capacity violation check ---\n";
    try {
        // Alice + Bob + Carol = 3 participants. This exceeds ROOM_B's capacity (2)
        scheduler.scheduleMeeting(aliceId, {bobId, carolId}, "Big Team Sync", date,
                                  11 * 60, 12 * 60, "ROOM_B");
        cout << "  ERROR: Seating capacity exceed exception nahi aayi!\n";
    } catch (const exception& ex) {
        cout << "  Expected capacity error: " << ex.what() << "\n";
    }

    // 8. Verify Room Overlap Booking block (R3)
    cout << "\n--- Room Overlapping Booking verification ---\n";
    try {
        // Alice attempts to book ROOM_B during Alice + Bob's sprint planning time (10:00 - 11:00)
        scheduler.scheduleMeeting(carolId, {bobId}, "Parallel meeting", date,
                                  10 * 60 + 15, 10 * 60 + 45, "ROOM_B");
        cout << "  ERROR: Room double booking allowed! Ye fail hona chahiye tha.\n";
    } catch (const exception& ex) {
        cout << "  Expected booking conflict: " << ex.what() << "\n";
    }

    // 9. Standard Conflict rejection test (Attendee busy) (FR4)
    cout << "\n--- Conflict rejected (Bob busy in strictMode) ---\n";
    try {
        // Bob is busy because Bob has "Sprint planning" meeting from 10:00 to 11:00.
        // We try to schedule another meeting for Bob at 10:15-10:45 with strictMode = true.
        scheduler.scheduleMeeting(aliceId, {bobId}, "Double-book Bob", date, 10 * 60 + 15,
                                  10 * 60 + 45, "ROOM_A", true /* strictMode = true */);
        cout << "  ERROR: strictMode me constraint skip ho gaya!\n";
    } catch (const exception& ex) {
        cout << "  Expected conflict error: " << ex.what() << "\n";
    }

    // 10. Flexible invitation scheduling test (R5: Invite even if unavailable/busy)
    cout << "\n--- Flexible scheduling (strictMode = false) ---\n";
    // Bob has "Sprint planning" (10:00-11:00). Carol (free at 10:15-10:45) invites Bob to a meeting.
    // Carol is available 11:00-15:00. Wait, Carol is NOT available at 10:15-10:45.
    // Let's use Alice as the organizer because Alice is available 9:00-12:00.
    // Wait, Alice is already busy in "Sprint planning" (10:00-11:00).
    // Let's schedule at 9:15 - 9:45 where Alice (organizer) is available & free, but Bob (attendee) is NOT available (starts at 10:00).
    Meeting mFlexible1 = scheduler.scheduleMeeting(aliceId, {bobId}, "Optional Morning Sync", date,
                                                   9 * 60 + 15, 9 * 60 + 45, "ROOM_A", false /* strictMode = false */);
    cout << "  " << mFlexible1.getMeetingId() << " scheduled status is: " << statusToString(mFlexible1.getStatus()) << "\n";
    
    // Check initial invitation status on calendar. Bob should be PENDING.
    for (const Meeting& m : scheduler.listMeetingsForUser(bobId, date)) {
        if (m.getMeetingId() == mFlexible1.getMeetingId()) {
            auto statuses = m.getAttendeeStatuses();
            cout << "  Bob's initial invitation status for " << m.getMeetingId() << " is: "
                 << inviteStatusToString(statuses[bobId]) << "\n";
        }
    }

    // 11. Invitation Response Flow (R5: accept/decline)
    cout << "\n--- Bob responds to Flexible Invitation (Decline scenario) ---\n";
    scheduler.respondToInvitation(mFlexible1.getMeetingId(), bobId, InvitationStatus::DECLINED);
    
    // Verify Bob's status becomes DECLINED
    for (const Meeting& m : scheduler.listMeetingsForUser(bobId, date)) {
        if (m.getMeetingId() == mFlexible1.getMeetingId()) {
            auto statuses = m.getAttendeeStatuses();
            cout << "  Bob's updated invitation status: " << inviteStatusToString(statuses[bobId]) << "\n";
        }
    }

    // Let's test invitation accept scenario where attendee is free
    cout << "\n--- Carol schedules meeting with Alice (flexible) and Alice accepts ---\n";
    // Carol is available 11:00-15:00, Alice is available 9:00-12:00. Let's schedule at 11:15-11:45. Both are free.
    Meeting mFlexible2 = scheduler.scheduleMeeting(carolId, {aliceId}, "Weekly Align", date,
                                                   11 * 60 + 15, 11 * 60 + 45, "ROOM_A", false);
    scheduler.respondToInvitation(mFlexible2.getMeetingId(), aliceId, InvitationStatus::ACCEPTED);

    // Now Alice is busy at 11:15-11:45. Let's test accept conflict error:
    cout << "\n--- Alice accepts conflicting flexible invitation ---\n";
    // Bob schedules a flexible meeting with Alice at 11:15-11:45. Alice gets invitation as PENDING.
    // We pass empty room ID "" to let it auto-allocate ROOM_B (since ROOM_A is occupied by Weekly Align).
    Meeting mConflicting = scheduler.scheduleMeeting(bobId, {aliceId}, "Urgent Catchup", date,
                                                     11 * 60 + 15, 11 * 60 + 45, "", false);
    try {
        // Alice tries to ACCEPT the "Urgent Catchup", but Alice is already in "Weekly Align" (ACCEPTED). This should fail.
        scheduler.respondToInvitation(mConflicting.getMeetingId(), aliceId, InvitationStatus::ACCEPTED);
        cout << "  ERROR: Conflicting accept allowed!\n";
    } catch (const exception& ex) {
        cout << "  Expected accept conflict error: " << ex.what() << "\n";
    }

    // 12. Book from dynamic free slot allocation
    cout << "\n--- Book from free slot list (Automatic Room Allocation) ---\n";
    if (!slots.empty()) {
        const TimeSlot& pick = slots.back();
        Meeting m2 = scheduler.scheduleMeeting(bobId, {aliceId, carolId}, "Design review", pick.getDate(),
                                               pick.getStartMinutes(), pick.getEndMinutes());
        cout << "  " << m2.getMeetingId() << " automatically assigned Room ID: " << m2.getRoomId()
             << " for timing " << formatMinutes(m2.getStartMinutes()) << "-" << formatMinutes(m2.getEndMinutes()) << "\n";
    }

    // 13. Calendar verification (R6: List meetings on calendar for Alice)
    cout << "\n--- Alice's Calendar Meetings on " << date << " ---\n";
    const Calendar& aliceCalendar = scheduler.getUser(aliceId).getCalendar();
    for (const string& meetingId : aliceCalendar.getMeetingIds()) {
        // Fetch detail
        for (const Meeting& m : scheduler.listMeetingsForUser(aliceId, date)) {
            if (m.getMeetingId() == meetingId) {
                cout << "  Calendar Item -> ID: " << m.getMeetingId() << ", Title: '" << m.getTitle()
                     << "', Status: " << statusToString(m.getStatus()) << ", Room: " << m.getRoomId() << "\n";
            }
        }
    }

    // 14. Cancel Meeting flow
    cout << "\n--- Cancel Sprint Planning ---\n";
    scheduler.cancelMeeting(m1.getMeetingId(), bobId);
    cout << "  Meeting " << m1.getMeetingId() << " cancelled by Bob.\n";

    // 15. Verify Mutual slots after cancellation
    cout << "\n--- Slots after cancel (Bob free again at 10-11) ---\n";
    slots = scheduler.findMutualFreeSlots({aliceId, bobId}, date, 30);
    cout << "  Available Slots Count: " << slots.size() << "\n";

    cout << "\nDemo execution complete successfully.\n";
    return 0;
}
