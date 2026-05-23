#ifndef TASK_SCHEDULER_LLD_MODELS_JOB_H
#define TASK_SCHEDULER_LLD_MODELS_JOB_H

#include <functional>
#include <string>

#include "../enums/JobPriority.h"
#include "../enums/JobStatus.h"

namespace task_scheduler_lld {

using JobTask = std::function<void()>;

struct Job {
    std::string id;
    std::string name;
    JobTask task;
    JobPriority priority{JobPriority::NORMAL};
    JobStatus status{JobStatus::PENDING};
    long long scheduledAtEpochMs{0};
    int maxRetries{0};
    int attempts{0};
    long long retryDelayMs{1000};
    std::string lastError;

    bool isTerminal() const {
        return status == JobStatus::COMPLETED || status == JobStatus::FAILED ||
               status == JobStatus::CANCELLED;
    }
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_MODELS_JOB_H
