#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "core/MeetingSchedulerSystem.h"
#include "enums/MeetingStatus.h"

using namespace std;
using namespace meeting_scheduler_lld;

static string formatMinutes(int minutes) {
    int h = minutes / 60;
    int m = minutes % 60;
    ostringstream oss;
    oss << setfill('0') << setw(2) << h << ":" << setw(2) << m;
    return oss.str();
}

static string statusToString(MeetingStatus status) {
    return status == MeetingStatus::SCHEDULED ? "SCHEDULED" : "CANCELLED";
}

int main() {
    cout << "====== Meeting Scheduler LLD Demo ======\n\n";

    MeetingSchedulerSystem scheduler;
    const string date = "2026-05-25";

    const string aliceId =
        scheduler.registerUser("Alice", "alice@work.com", "Asia/Kolkata");
    const string bobId = scheduler.registerUser("Bob", "bob@work.com", "Asia/Kolkata");
    const string carolId = scheduler.registerUser("Carol", "carol@work.com", "Asia/Kolkata");

    scheduler.setAvailability(aliceId, date, 9 * 60, 12 * 60);
    scheduler.setAvailability(aliceId, date, 14 * 60, 17 * 60);
    scheduler.setAvailability(bobId, date, 10 * 60, 16 * 60);
    scheduler.setAvailability(carolId, date, 11 * 60, 15 * 60);

    cout << "--- Mutual free slots (60 min) ---\n";
    vector<TimeSlot> slots =
        scheduler.findMutualFreeSlots({aliceId, bobId, carolId}, date, 60);
    for (const TimeSlot& slot : slots) {
        cout << "  " << slot.getDate() << " " << formatMinutes(slot.getStartMinutes()) << "-"
             << formatMinutes(slot.getEndMinutes()) << "\n";
    }

    cout << "\n--- Schedule 1:1 (Alice + Bob) ---\n";
    Meeting m1 = scheduler.scheduleMeeting(aliceId, {bobId}, "Sprint planning", date,
                                           10 * 60, 11 * 60);
    cout << "  " << m1.getMeetingId() << " " << m1.getTitle() << " "
         << formatMinutes(m1.getStartMinutes()) << "-" << formatMinutes(m1.getEndMinutes())
         << " " << statusToString(m1.getStatus()) << "\n";

    cout << "\n--- Conflict rejected (Bob busy) ---\n";
    try {
        scheduler.scheduleMeeting(aliceId, {bobId}, "Double-book Bob", date, 10 * 60 + 15,
                                  10 * 60 + 45);
        cout << "  ERROR: should have thrown\n";
    } catch (const exception& ex) {
        cout << "  Expected: " << ex.what() << "\n";
    }

    cout << "\n--- Book from free slot list ---\n";
    if (!slots.empty()) {
        const TimeSlot& pick = slots.back();
        Meeting m2 =
            scheduler.scheduleMeeting(bobId, {aliceId, carolId}, "Design review", pick.getDate(),
                                      pick.getStartMinutes(), pick.getEndMinutes());
        cout << "  " << m2.getMeetingId() << " booked " << formatMinutes(m2.getStartMinutes())
             << "-" << formatMinutes(m2.getEndMinutes()) << "\n";
    }

    cout << "\n--- Alice meetings on " << date << " ---\n";
    for (const Meeting& m : scheduler.listMeetingsForUser(aliceId, date)) {
        cout << "  " << m.getMeetingId() << " " << m.getTitle() << " "
             << statusToString(m.getStatus()) << "\n";
    }

    cout << "\n--- Cancel sprint planning ---\n";
    scheduler.cancelMeeting(m1.getMeetingId(), bobId);
    cout << "  Cancelled by Bob\n";

    cout << "\n--- Slots after cancel (Bob free again at 10-11) ---\n";
    slots = scheduler.findMutualFreeSlots({aliceId, bobId}, date, 30);
    cout << "  Count: " << slots.size() << " slots\n";

    cout << "\nDemo complete.\n";
    return 0;
}
