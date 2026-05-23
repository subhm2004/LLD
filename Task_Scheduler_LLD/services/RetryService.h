#ifndef TASK_SCHEDULER_LLD_SERVICES_RETRYSERVICE_H
#define TASK_SCHEDULER_LLD_SERVICES_RETRYSERVICE_H

#include <memory>
#include <string>

#include "../enums/JobStatus.h"
#include "../models/Job.h"
#include "../utils/Clock.h"

namespace task_scheduler_lld {

class RetryService {
public:
    explicit RetryService(IClock* clock) : clock_(clock) {}

    /** Returns true if job was rescheduled for retry; false if permanently failed. */
    bool scheduleRetryIfEligible(const std::shared_ptr<Job>& job, const std::string& errorMessage) {
        job->lastError = errorMessage;
        job->attempts++;

        if (job->attempts <= job->maxRetries) {
            job->status = JobStatus::RETRY_SCHEDULED;
            job->scheduledAtEpochMs = clock_->nowEpochMs() + job->retryDelayMs;
            return true;
        }

        job->status = JobStatus::FAILED;
        return false;
    }

private:
    IClock* clock_;
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_SERVICES_RETRYSERVICE_H
