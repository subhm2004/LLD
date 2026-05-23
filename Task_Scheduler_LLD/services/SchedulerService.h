#ifndef TASK_SCHEDULER_LLD_SERVICES_SCHEDULERSERVICE_H
#define TASK_SCHEDULER_LLD_SERVICES_SCHEDULERSERVICE_H

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "../models/Job.h"
#include "../strategies/ISchedulingStrategy.h"
#include "../utils/Clock.h"

namespace task_scheduler_lld {

/**
 * Holds jobs waiting for their scheduled time, then moves due jobs into the ready queue
 * (via ISchedulingStrategy).
 */
class SchedulerService {
public:
    explicit SchedulerService(ISchedulingStrategy* readyQueue, IClock* clock)
        : readyQueue_(readyQueue), clock_(clock) {}

    void schedule(const std::shared_ptr<Job>& job) {
        std::lock_guard<std::mutex> lock(mutex_);
        delayedHeap_.push(job);
    }

    void promoteDueJobs() {
        std::lock_guard<std::mutex> lock(mutex_);
        const long long now = clock_->nowEpochMs();
        while (!delayedHeap_.empty()) {
            const std::shared_ptr<Job>& top = delayedHeap_.top();
            if (top->scheduledAtEpochMs > now) {
                break;
            }
            std::shared_ptr<Job> job = delayedHeap_.top();
            delayedHeap_.pop();
            readyQueue_->enqueue(job);
        }
    }

    size_t delayedCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return delayedHeap_.size();
    }

private:
    struct DelayedCompare {
        bool operator()(const std::shared_ptr<Job>& a, const std::shared_ptr<Job>& b) const {
            return a->scheduledAtEpochMs > b->scheduledAtEpochMs;
        }
    };

    ISchedulingStrategy* readyQueue_;
    IClock* clock_;
    mutable std::mutex mutex_;
    std::priority_queue<std::shared_ptr<Job>, std::vector<std::shared_ptr<Job>>, DelayedCompare>
        delayedHeap_;
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_SERVICES_SCHEDULERSERVICE_H
