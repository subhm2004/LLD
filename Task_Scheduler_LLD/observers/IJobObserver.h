#ifndef TASK_SCHEDULER_LLD_OBSERVERS_IJOBOBSERVER_H
#define TASK_SCHEDULER_LLD_OBSERVERS_IJOBOBSERVER_H

#include <string>

#include "../enums/JobStatus.h"

namespace task_scheduler_lld {

class IJobObserver {
public:
    virtual ~IJobObserver() = default;
    virtual void onJobStatusChanged(const std::string& jobId, const std::string& jobName,
                                    JobStatus status, const std::string& detail) = 0;
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_OBSERVERS_IJOBOBSERVER_H
