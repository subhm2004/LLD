#ifndef TASK_SCHEDULER_LLD_STRATEGIES_FIFOSCHEDULINGSTRATEGY_H
#define TASK_SCHEDULER_LLD_STRATEGIES_FIFOSCHEDULINGSTRATEGY_H

#include <memory>
#include <queue>

#include "ISchedulingStrategy.h"

namespace task_scheduler_lld {

class FifoSchedulingStrategy : public ISchedulingStrategy {
public:
    void enqueue(std::shared_ptr<Job> job) override { queue_.push(std::move(job)); }

    std::shared_ptr<Job> dequeueReady() override {
        if (queue_.empty()) {
            return nullptr;
        }
        std::shared_ptr<Job> job = queue_.front();
        queue_.pop();
        return job;
    }

    bool empty() const override { return queue_.empty(); }

    size_t size() const override { return queue_.size(); }

private:
    std::queue<std::shared_ptr<Job>> queue_;
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_STRATEGIES_FIFOSCHEDULINGSTRATEGY_H
