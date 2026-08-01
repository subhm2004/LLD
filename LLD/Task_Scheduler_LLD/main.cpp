#include <atomic>
#include <iostream>
#include <string>
#include <thread>

#include "core/TaskSchedulerSystem.h"
#include "enums/JobPriority.h"
#include "enums/JobStatus.h"
#include "observers/ConsoleJobObserver.h"

using namespace std;
using namespace task_scheduler_lld;

static atomic<int> flakyAttempts{0};

int main() {
    cout << "====== Task / Job Scheduler LLD Demo ======\n\n";

    TaskSchedulerSystem scheduler;
    scheduler.setSimulatedNow(0);
    scheduler.addObserver(make_shared<ConsoleJobObserver>());

    cout << "--- Priority: CRITICAL should run before LOW (same time) ---\n";
    scheduler.submitJob(
        "send-receipt-email",
        []() { cout << "  [task] Email receipt sent\n"; },
        JobPriority::LOW);

    scheduler.submitJob(
        "sync-analytics",
        []() { cout << "  [task] Analytics synced\n"; },
        JobPriority::NORMAL);

    scheduler.submitJob(
        "fraud-check",
        []() { cout << "  [task] Fraud check completed\n"; },
        JobPriority::CRITICAL);

    scheduler.start(2);
    this_thread::sleep_for(chrono::milliseconds(400));

    cout << "\n--- Delayed job (runs after simulated +1500ms) ---\n";
    scheduler.submitJob(
        "nightly-backup",
        []() { cout << "  [task] Nightly backup finished\n"; },
        JobPriority::HIGH, 1500);

    cout << "\n--- Retry: fails twice, succeeds on 3rd attempt ---\n";
    flakyAttempts = 0;
    scheduler.submitJob(
        "payment-settlement",
        []() {
            const int attempt = ++flakyAttempts;
            cout << "  [task] Settlement attempt " << attempt << "\n";
            if (attempt < 3) {
                throw runtime_error("gateway timeout");
            }
            cout << "  [task] Settlement succeeded\n";
        },
        JobPriority::HIGH, 0, 3, 400);

    cout << "\n--- Cancel pending job before it runs ---\n";
    const string slowReportId = scheduler.submitJob(
        "weekly-report",
        []() { cout << "  [task] Weekly report generated\n"; },
        JobPriority::NORMAL, 5000);
    scheduler.cancelJob(slowReportId);
    cout << "  Cancelled " << slowReportId << " status="
         << jobStatusToString(scheduler.getJobStatus(slowReportId)) << "\n";

    cout << "\n--- Advancing simulated clock for delayed + retries ---\n";
    scheduler.waitForCompletion(8000, 40, 200);

    cout << "\n--- Final job statuses ---\n";
    for (const string& id :
         {"JOB_1", "JOB_2", "JOB_3", "JOB_4", "JOB_5", "JOB_6"}) {
        try {
            const auto job = scheduler.getJob(id);
            cout << "  " << id << " " << job->name << " -> "
                 << jobStatusToString(job->status);
            if (!job->lastError.empty() && job->status == JobStatus::FAILED) {
                cout << " (" << job->lastError << ")";
            }
            cout << " attempts=" << job->attempts << "\n";
        } catch (...) {
            // skip missing ids if counter differs
        }
    }

    scheduler.stop();
    cout << "\nDemo complete.\n";
    return 0;
}
