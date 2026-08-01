#include <iostream>
#include <string>

#include "core/LeaveRequestSystem.h"
#include "enums/ApproverRole.h"
#include "enums/LeaveStatus.h"
#include "enums/LeaveType.h"

using namespace std;
using namespace leave_request_lld;

static void printRequest(const LeaveRequest& req) {
    cout << "  " << req.requestId << " | " << leaveTypeToString(req.leaveType) << " | "
         << req.startDate << " -> " << req.endDate << " | " << req.workingDays << " day(s) | "
         << leaveStatusToString(req.status);
    if (req.status == LeaveStatus::APPROVED) {
        cout << " | by " << approverRoleToString(req.approvedBy);
    }
    if (!req.remarks.empty()) {
        cout << " | " << req.remarks;
    }
    cout << "\n";
}

int main() {
    cout << "====== Leave Request System LLD (Chain of Responsibility) ======\n\n";

    LeaveRequestSystem system;
    const string aliceId = system.registerEmployee("Alice", "Platform");
    const string bobId = system.registerEmployee("Bob", "Platform");

    cout << "--- 1 day: Team Lead approves (chain stops early) ---\n";
    const string lr1 = system.submitLeave(aliceId, LeaveType::CASUAL, "2026-06-02", "2026-06-02", 1);
    system.processApproval(lr1);
    printRequest(system.getLeaveRequest(lr1));

    cout << "\n--- 3 days: escalates past Team Lead, Manager approves ---\n";
    const string lr2 =
        system.submitLeave(aliceId, LeaveType::SICK, "2026-06-10", "2026-06-12", 3);
    system.processApproval(lr2);
    printRequest(system.getLeaveRequest(lr2));

    cout << "\n--- 5 days: HR approves (Team Lead & Manager pass along) ---\n";
    const string lr3 =
        system.submitLeave(bobId, LeaveType::EARNED, "2026-07-01", "2026-07-05", 5);
    system.processApproval(lr3);
    printRequest(system.getLeaveRequest(lr3));

    cout << "\n--- 15 days: Director approves (full chain traversal) ---\n";
    const string lr4 =
        system.submitLeave(bobId, LeaveType::UNPAID, "2026-08-01", "2026-08-15", 15);
    system.processApproval(lr4);
    printRequest(system.getLeaveRequest(lr4));

    cout << "\n--- 45 days: exceeds Director cap → REJECTED at chain end ---\n";
    const string lr5 =
        system.submitLeave(aliceId, LeaveType::EARNED, "2026-09-01", "2026-10-15", 45);
    system.processApproval(lr5);
    printRequest(system.getLeaveRequest(lr5));

    cout << "\n--- Cancel PENDING before approval chain runs ---\n";
    const string lr6 =
        system.submitLeave(aliceId, LeaveType::CASUAL, "2026-06-20", "2026-06-22", 2);
    system.cancelLeave(lr6);
    printRequest(system.getLeaveRequest(lr6));

    cout << "\n--- Alice leave history ---\n";
    for (const LeaveRequest& req : system.listEmployeeLeaves(aliceId)) {
        printRequest(req);
    }

    cout << "\nDemo complete.\n";
    return 0;
}
