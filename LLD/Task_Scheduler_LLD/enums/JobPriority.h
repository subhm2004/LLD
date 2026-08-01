#ifndef TASK_SCHEDULER_LLD_ENUMS_JOBPRIORITY_H
#define TASK_SCHEDULER_LLD_ENUMS_JOBPRIORITY_H

namespace task_scheduler_lld {

enum class JobPriority {
    LOW = 0,
    NORMAL = 1,
    HIGH = 2,
    CRITICAL = 3
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_ENUMS_JOBPRIORITY_H
