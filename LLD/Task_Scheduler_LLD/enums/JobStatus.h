#ifndef TASK_SCHEDULER_LLD_ENUMS_JOBSTATUS_H
#define TASK_SCHEDULER_LLD_ENUMS_JOBSTATUS_H

namespace task_scheduler_lld {

enum class JobStatus {
    PENDING,
    SCHEDULED,
    RUNNING,
    COMPLETED,
    FAILED,
    CANCELLED,
    RETRY_SCHEDULED
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_ENUMS_JOBSTATUS_H
