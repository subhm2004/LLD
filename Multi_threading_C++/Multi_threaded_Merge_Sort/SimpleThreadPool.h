#ifndef MULTI_THREADING_MERGE_SORT_SIMPLETHREADPOOL_H
#define MULTI_THREADING_MERGE_SORT_SIMPLETHREADPOOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace merge_sort_demo {

/** Minimal thread pool for merge-sort tasks (C++17). */
class SimpleThreadPool {
public:
    explicit SimpleThreadPool(size_t num_threads) : stop_(false) {
        workers_.reserve(num_threads);
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this] { workerLoop(); });
        }
    }

    ~SimpleThreadPool() { shutdown(); }

    template <typename F>
    auto submit(F&& task) -> std::future<std::invoke_result_t<F>> {
        using Ret = std::invoke_result_t<F>;
        auto packaged =
            std::make_shared<std::packaged_task<Ret()>>(std::forward<F>(task));
        std::future<Ret> result = packaged->get_future();
        {
            std::lock_guard<std::mutex> lock(queue_mtx_);
            if (stop_) {
                throw std::runtime_error("pool stopped");
            }
            tasks_.emplace([packaged]() { (*packaged)(); });
        }
        cv_.notify_one();
        return result;
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(queue_mtx_);
            if (stop_) {
                return;
            }
            stop_ = true;
        }
        cv_.notify_all();
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

private:
    void workerLoop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mtx_);
                cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                if (stop_ && tasks_.empty()) {
                    return;
                }
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mtx_;
    std::condition_variable cv_;
    bool stop_;
};

}  // namespace merge_sort_demo

#endif  // MULTI_THREADING_MERGE_SORT_SIMPLETHREADPOOL_H
