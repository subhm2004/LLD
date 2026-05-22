#ifndef BOUNDED_BUFFER_H
#define BOUNDED_BUFFER_H

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

/**
 * Thread-safe bounded buffer — classic Producer-Consumer core.
 * C++17: dual condition_variable (not full / not empty) + optional shutdown.
 */
template <typename T> class BoundedBuffer {
public:
  explicit BoundedBuffer(size_t capacity) : capacity_(capacity) {}

  void produce(T item) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_not_full_.wait(
        lock, [this] { return buffer_.size() < capacity_ || shutdown_; });
    if (shutdown_) {
      return;
    }
    buffer_.push(std::move(item));
    cv_not_empty_.notify_one();
  }

  std::optional<T> consume() {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_not_empty_.wait(lock, [this] { return !buffer_.empty() || shutdown_; });
    if (buffer_.empty()) {
      return std::nullopt;
    }
    T item = std::move(buffer_.front());
    buffer_.pop();
    cv_not_full_.notify_one();
    return item;
  }

  void signal_shutdown() {
    {
      std::lock_guard<std::mutex> lock(mtx_);
      shutdown_ = true;
    }
    cv_not_empty_.notify_all();
    cv_not_full_.notify_all();
  }

  size_t size() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return buffer_.size();
  }

  size_t capacity() const { return capacity_; }

  bool is_shutdown() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return shutdown_;
  }

private:
  size_t capacity_;
  std::queue<T> buffer_;
  mutable std::mutex mtx_;
  std::condition_variable cv_not_full_;
  std::condition_variable cv_not_empty_;
  bool shutdown_{false};
};
#endif // BOUNDED_BUFFER_H