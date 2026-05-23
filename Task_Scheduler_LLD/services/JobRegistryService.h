#ifndef TASK_SCHEDULER_LLD_SERVICES_JOBREGISTRYSERVICE_H
#define TASK_SCHEDULER_LLD_SERVICES_JOBREGISTRYSERVICE_H

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/Job.h"

namespace task_scheduler_lld {

class JobRegistryService {
public:
    void registerJob(const std::shared_ptr<Job>& job) {
        if (jobs_.count(job->id) != 0) {
            throw std::runtime_error("job already exists: " + job->id);
        }
        jobs_[job->id] = job;
    }

    std::shared_ptr<Job> get(const std::string& jobId) const {
        auto it = jobs_.find(jobId);
        if (it == jobs_.end()) {
            throw std::runtime_error("job not found: " + jobId);
        }
        return it->second;
    }

    bool exists(const std::string& jobId) const { return jobs_.count(jobId) != 0; }

    std::vector<std::shared_ptr<Job>> listAll() const {
        std::vector<std::shared_ptr<Job>> result;
        result.reserve(jobs_.size());
        for (const auto& entry : jobs_) {
            result.push_back(entry.second);
        }
        return result;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Job>> jobs_;
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_SERVICES_JOBREGISTRYSERVICE_H
