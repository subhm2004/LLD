#ifndef TASK_SCHEDULER_LLD_STRATEGIES_ISCHEDULINGSTRATEGY_H
#define TASK_SCHEDULER_LLD_STRATEGIES_ISCHEDULINGSTRATEGY_H

#include <memory>
#include <queue>
#include <vector>

#include "../models/Job.h"

namespace task_scheduler_lld {

class ISchedulingStrategy {
public:
    virtual ~ISchedulingStrategy() = default;

    virtual void enqueue(std::shared_ptr<Job> job) = 0;

    virtual std::shared_ptr<Job> dequeueReady() = 0;

    virtual bool empty() const = 0;

    virtual size_t size() const = 0;
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_STRATEGIES_ISCHEDULINGSTRATEGY_H
