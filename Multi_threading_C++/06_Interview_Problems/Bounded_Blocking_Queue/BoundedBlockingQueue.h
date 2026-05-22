#ifndef MULTI_THREADING_BOUNDED_BLOCKING_QUEUE_H
#define MULTI_THREADING_BOUNDED_BLOCKING_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace bounded_queue {

/**
 * Thread-safe bounded blocking queue — classic interview + producer-consumer core.
 */
template <typename T>
class BoundedBlockingQueue {
public:
    explicit BoundedBlockingQueue(int capacity) : capacity_(capacity) {}

    void enqueue(T item) {
        std::unique_lock<std::mutex> lock(mtx_);
        not_full_.wait(lock, [this] { return queue_.size() < static_cast<size_t>(capacity_); });
        queue_.push(std::move(item));
        not_empty_.notify_one();
    }

    T dequeue() {
        std::unique_lock<std::mutex> lock(mtx_);
        not_empty_.wait(lock, [this] { return !queue_.empty(); });
        T item = std::move(queue_.front());
        queue_.pop();
        not_full_.notify_one();
        return item;
    }

    std::optional<T> tryDequeue() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (queue_.empty()) {
            return std::nullopt;
        }
        T item = std::move(queue_.front());
        queue_.pop();
        not_full_.notify_one();
        return item;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.size();
    }

private:
    int capacity_;
    std::queue<T> queue_;
    mutable std::mutex mtx_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
};

}  // namespace bounded_queue

#endif  // MULTI_THREADING_BOUNDED_BLOCKING_QUEUE_H
