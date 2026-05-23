#ifndef TASK_SCHEDULER_LLD_STRATEGIES_PRIORITYSCHEDULINGSTRATEGY_H
#define TASK_SCHEDULER_LLD_STRATEGIES_PRIORITYSCHEDULINGSTRATEGY_H

#include <functional>
#include <memory>
#include <queue>
#include <vector>

#include "ISchedulingStrategy.h"

namespace task_scheduler_lld {

/** Higher priority first; tie-break by earlier scheduled time, then FIFO by id. */
class PrioritySchedulingStrategy : public ISchedulingStrategy {
public:
    void enqueue(std::shared_ptr<Job> job) override {
        heap_.push(std::move(job));
    }

    std::shared_ptr<Job> dequeueReady() override {
        if (heap_.empty()) {
            return nullptr;
        }
        std::shared_ptr<Job> job = heap_.top();
        heap_.pop();
        return job;
    }

    bool empty() const override { return heap_.empty(); }

    size_t size() const override { return heap_.size(); }

private:
    struct Compare {
        bool operator()(const std::shared_ptr<Job>& a, const std::shared_ptr<Job>& b) const {
            if (static_cast<int>(a->priority) != static_cast<int>(b->priority)) {
                return static_cast<int>(a->priority) < static_cast<int>(b->priority);
            }
            if (a->scheduledAtEpochMs != b->scheduledAtEpochMs) {
                return a->scheduledAtEpochMs > b->scheduledAtEpochMs;
            }
            return a->id > b->id;
        }
    };

    std::priority_queue<std::shared_ptr<Job>, std::vector<std::shared_ptr<Job>>, Compare> heap_;
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_STRATEGIES_PRIORITYSCHEDULINGSTRATEGY_H
