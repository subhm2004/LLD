#ifndef TASK_SCHEDULER_LLD_FACTORIES_JOBFACTORY_H
#define TASK_SCHEDULER_LLD_FACTORIES_JOBFACTORY_H

#include <memory>
#include <stdexcept>
#include <string>

#include "../models/Job.h"

namespace task_scheduler_lld {

class JobFactory {
public:
    static std::shared_ptr<Job> create(const std::string& id, const std::string& name, JobTask task,
                                       JobPriority priority, long long scheduledAtEpochMs,
                                       int maxRetries, long long retryDelayMs) {
        if (id.empty()) {
            throw std::invalid_argument("job id required");
        }
        if (name.empty()) {
            throw std::invalid_argument("job name required");
        }
        if (!task) {
            throw std::invalid_argument("job task required");
        }
        if (maxRetries < 0) {
            throw std::invalid_argument("maxRetries must be >= 0");
        }
        if (retryDelayMs < 0) {
            throw std::invalid_argument("retryDelayMs must be >= 0");
        }

        auto job = std::make_shared<Job>();
        job->id = id;
        job->name = name;
        job->task = std::move(task);
        job->priority = priority;
        job->status = JobStatus::SCHEDULED;
        job->scheduledAtEpochMs = scheduledAtEpochMs;
        job->maxRetries = maxRetries;
        job->retryDelayMs = retryDelayMs;
        return job;
    }
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_FACTORIES_JOBFACTORY_H
