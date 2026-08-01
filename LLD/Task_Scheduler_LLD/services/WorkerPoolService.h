#ifndef TASK_SCHEDULER_LLD_SERVICES_WORKERPOOLSERVICE_H
#define TASK_SCHEDULER_LLD_SERVICES_WORKERPOOLSERVICE_H

#include <atomic>
#include <condition_variable>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "../enums/JobStatus.h"
#include "../models/Job.h"
#include "../observers/IJobObserver.h"
#include "../strategies/ISchedulingStrategy.h"
#include "RetryService.h"
#include "SchedulerService.h"

namespace task_scheduler_lld {

class WorkerPoolService {
public:
    WorkerPoolService(ISchedulingStrategy* readyQueue, SchedulerService* scheduler,
                      RetryService* retryService)
        : readyQueue_(readyQueue), scheduler_(scheduler), retryService_(retryService) {}

    ~WorkerPoolService() { stop(); }

    void addObserver(std::shared_ptr<IJobObserver> observer) {
        observers_.push_back(std::move(observer));
    }

    void start(int workerCount) {
        if (running_) {
            return;
        }
        if (workerCount < 1) {
            throw std::invalid_argument("workerCount must be >= 1");
        }
        running_ = true;
        for (int i = 0; i < workerCount; ++i) {
            workers_.emplace_back([this, i]() { workerLoop(i); });
        }
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        cv_.notify_all();
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        workers_.clear();
    }

    void notifyWorkAvailable() {
        cv_.notify_all();
    }

private:
    ISchedulingStrategy* readyQueue_;
    SchedulerService* scheduler_;
    RetryService* retryService_;
    std::vector<std::shared_ptr<IJobObserver>> observers_;

    std::vector<std::thread> workers_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool running_{false};

    void workerLoop(int workerId) {
        while (true) {
            std::shared_ptr<Job> job;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this]() {
                    if (!running_) {
                        return true;
                    }
                    scheduler_->promoteDueJobs();
                    return !readyQueue_->empty();
                });

                if (!running_) {
                    return;
                }

                job = readyQueue_->dequeueReady();
                if (!job) {
                    continue;
                }
            }

            executeJob(job, workerId);
        }
    }

    void executeJob(const std::shared_ptr<Job>& job, int workerId) {
        if (job->status == JobStatus::CANCELLED) {
            return;
        }

        job->status = JobStatus::RUNNING;
        notifyObservers(job, "worker-" + std::to_string(workerId));

        try {
            job->task();
            job->status = JobStatus::COMPLETED;
            notifyObservers(job, "success");
        } catch (const std::exception& ex) {
            const bool willRetry =
                retryService_->scheduleRetryIfEligible(job, ex.what());
            if (willRetry) {
                notifyObservers(job, "retry in " + std::to_string(job->retryDelayMs) + "ms");
                scheduler_->schedule(job);
                notifyWorkAvailable();
            } else {
                notifyObservers(job, "failed permanently");
            }
        }
    }

    void notifyObservers(const std::shared_ptr<Job>& job, const std::string& detail) {
        for (const auto& observer : observers_) {
            observer->onJobStatusChanged(job->id, job->name, job->status, detail);
        }
    }
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_SERVICES_WORKERPOOLSERVICE_H
