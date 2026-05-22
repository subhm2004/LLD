#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/**
 * Fixed-size thread pool — workers wait on task queue, reuse threads.
 * C++17: mutex + condition_variable + queue<function<void()>>
 */
class ThreadPool {
public:
  explicit ThreadPool(size_t num_threads) : stop_(false) {
    workers_.reserve(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
      workers_.emplace_back([this, i] { worker_loop(i); });
    }
  }

  ~ThreadPool() { shutdown(); }

  void enqueue(std::function<void()> task) {
    {
      std::lock_guard<std::mutex> lock(queue_mtx_);
      if (stop_) {
        return;
      }
      tasks_.push(std::move(task));
    }
    cv_.notify_one();
  }

  template <typename F, typename... Args>
  auto submit(F &&f, Args &&...args)
      -> std::future<std::invoke_result_t<F, Args...>> {
    using Ret = std::invoke_result_t<F, Args...>;
    auto bound = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto task_ptr =
        std::make_shared<std::packaged_task<Ret()>>(std::move(bound));
    std::future<Ret> fut = task_ptr->get_future();
    enqueue([task_ptr]() { (*task_ptr)(); });
    return fut;
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
    for (auto &w : workers_) {
      if (w.joinable()) {
        w.join();
      }
    }
    workers_.clear();
  }

  size_t pending_tasks() const {
    std::lock_guard<std::mutex> lock(queue_mtx_);
    return tasks_.size();
  }

  size_t worker_count() const { return workers_.size(); }

  bool is_stopped() const { return stop_.load(); }

  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

private:
  void worker_loop(size_t worker_id) {
    (void)worker_id;
    while (true) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(queue_mtx_);
        cv_.wait(lock, [this] { return !tasks_.empty() || stop_; });

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
  mutable std::mutex queue_mtx_;
  std::condition_variable cv_;
  std::queue<std::function<void()>> tasks_;
  std::atomic<bool> stop_{false};
};
#endif // THREAD_POOL_H