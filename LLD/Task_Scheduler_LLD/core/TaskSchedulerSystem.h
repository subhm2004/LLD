#ifndef TASK_SCHEDULER_LLD_CORE_TASKSCHEDULERSYSTEM_H
#define TASK_SCHEDULER_LLD_CORE_TASKSCHEDULERSYSTEM_H

#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "../enums/JobPriority.h"
#include "../enums/JobStatus.h"
#include "../factories/JobFactory.h"
#include "../models/Job.h"
#include "../observers/IJobObserver.h"
#include "../services/JobRegistryService.h"
#include "../services/RetryService.h"
#include "../services/SchedulerService.h"
#include "../services/WorkerPoolService.h"
#include "../strategies/PrioritySchedulingStrategy.h"
#include "../utils/Clock.h"

namespace task_scheduler_lld {

class TaskSchedulerSystem {
public:
    explicit TaskSchedulerSystem(IClock* clock = nullptr)
        : ownedClock_(clock ? nullptr : std::make_unique<SimulatedClock>()),
          clock_(clock ? clock : ownedClock_.get()),
          retryService_(clock_),
          schedulerService_(&readyStrategy_, clock_),
          workerPool_(&readyStrategy_, &schedulerService_, &retryService_) {}

    void setSimulatedNow(long long epochMs) {
        if (auto* simulated = dynamic_cast<SimulatedClock*>(clock_)) {
            simulated->setNow(epochMs);
            workerPool_.notifyWorkAvailable();
        }
    }

    void advanceSimulatedTime(long long deltaMs) {
        if (auto* simulated = dynamic_cast<SimulatedClock*>(clock_)) {
            simulated->advanceMs(deltaMs);
            workerPool_.notifyWorkAvailable();
        }
    }

    long long nowMs() const { return clock_->nowEpochMs(); }

    void addObserver(std::shared_ptr<IJobObserver> observer) {
        observers_.push_back(observer);
        workerPool_.addObserver(observer);
    }

    void start(int workerCount) { workerPool_.start(workerCount); }

    void stop() { workerPool_.stop(); }

    std::string submitJob(const std::string& name, JobTask task,
                          JobPriority priority = JobPriority::NORMAL, long long delayMs = 0,
                          int maxRetries = 0, long long retryDelayMs = 500) {
        const std::string jobId = "JOB_" + std::to_string(++jobCounter_);
        const long long runAt = clock_->nowEpochMs() + delayMs;
        auto job =
            JobFactory::create(jobId, name, std::move(task), priority, runAt, maxRetries, retryDelayMs);
        registry_.registerJob(job);
        schedulerService_.schedule(job);
        workerPool_.notifyWorkAvailable();
        return jobId;
    }

    void cancelJob(const std::string& jobId) {
        auto job = registry_.get(jobId);
        if (job->isTerminal()) {
            throw std::runtime_error("cannot cancel terminal job: " + jobId);
        }
        job->status = JobStatus::CANCELLED;
    }

    JobStatus getJobStatus(const std::string& jobId) const { return registry_.get(jobId)->status; }

    std::shared_ptr<Job> getJob(const std::string& jobId) const { return registry_.get(jobId); }

    /** Let worker threads drain work; optional simulated time jumps between polls. */
    void waitForCompletion(int maxWaitMs = 5000, int pollMs = 25, long long simulatedStepMs = 0) {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(maxWaitMs);
        while (std::chrono::steady_clock::now() < deadline) {
            if (allJobsTerminal()) {
                return;
            }
            if (simulatedStepMs > 0) {
                advanceSimulatedTime(simulatedStepMs);
            }
            workerPool_.notifyWorkAvailable();
            std::this_thread::sleep_for(std::chrono::milliseconds(pollMs));
        }
    }

private:
    std::unique_ptr<IClock> ownedClock_;
    IClock* clock_;
    PrioritySchedulingStrategy readyStrategy_;
    JobRegistryService registry_;
    RetryService retryService_;
    SchedulerService schedulerService_;
    WorkerPoolService workerPool_;

    std::vector<std::shared_ptr<IJobObserver>> observers_;
    int jobCounter_{0};

    bool allJobsTerminal() const {
        for (const auto& job : registry_.listAll()) {
            if (!job->isTerminal()) {
                return false;
            }
        }
        return true;
    }
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_CORE_TASKSCHEDULERSYSTEM_H
