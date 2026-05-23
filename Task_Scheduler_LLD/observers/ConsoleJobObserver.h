#ifndef TASK_SCHEDULER_LLD_OBSERVERS_CONSOLEJOBOBSERVER_H
#define TASK_SCHEDULER_LLD_OBSERVERS_CONSOLEJOBOBSERVER_H

#include <iostream>
#include <string>

#include "IJobObserver.h"

namespace task_scheduler_lld {

inline std::string jobStatusToString(JobStatus status) {
    switch (status) {
        case JobStatus::PENDING:
            return "PENDING";
        case JobStatus::SCHEDULED:
            return "SCHEDULED";
        case JobStatus::RUNNING:
            return "RUNNING";
        case JobStatus::COMPLETED:
            return "COMPLETED";
        case JobStatus::FAILED:
            return "FAILED";
        case JobStatus::CANCELLED:
            return "CANCELLED";
        case JobStatus::RETRY_SCHEDULED:
            return "RETRY_SCHEDULED";
    }
    return "UNKNOWN";
}

class ConsoleJobObserver : public IJobObserver {
public:
    void onJobStatusChanged(const std::string& jobId, const std::string& jobName,
                            JobStatus status, const std::string& detail) override {
        std::cout << "[Observer] " << jobId << " (" << jobName << ") -> "
                  << jobStatusToString(status);
        if (!detail.empty()) {
            std::cout << " | " << detail;
        }
        std::cout << "\n";
    }
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_OBSERVERS_CONSOLEJOBOBSERVER_H
