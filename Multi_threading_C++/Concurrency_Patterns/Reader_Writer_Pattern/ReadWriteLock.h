#pragma once

#include <condition_variable>
#include <mutex>

/**
 * Reader-preference read-write lock (educational).
 * Many readers OR one writer — not both.
 */
class ReadWriteLock {
public:
    void lock_shared() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !writer_active_; });
        ++readers_;
    }

    void unlock_shared() {
        std::unique_lock<std::mutex> lock(mtx_);
        --readers_;
        if (readers_ == 0) {
            cv_.notify_all();
        }
    }

    void lock() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return readers_ == 0 && !writer_active_; });
        writer_active_ = true;
    }

    void unlock() {
        std::unique_lock<std::mutex> lock(mtx_);
        writer_active_ = false;
        cv_.notify_all();
    }

    int readers() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return readers_;
    }

    bool writer_active() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return writer_active_;
    }

private:
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    int readers_{0};
    bool writer_active_{false};
};

/**
 * Writer-preference: once writer waits, new readers block.
 */
class WriterPreferenceRWLock {
public:
    void lock_shared() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] {
            return !writer_active_ && !writer_waiting_;
        });
        ++readers_;
    }

    void unlock_shared() {
        std::unique_lock<std::mutex> lock(mtx_);
        --readers_;
        if (readers_ == 0) {
            cv_.notify_all();
        }
    }

    void lock() {
        std::unique_lock<std::mutex> lock(mtx_);
        writer_waiting_ = true;
        cv_.wait(lock, [this] { return readers_ == 0 && !writer_active_; });
        writer_active_ = true;
        writer_waiting_ = false;
    }

    void unlock() {
        std::unique_lock<std::mutex> lock(mtx_);
        writer_active_ = false;
        cv_.notify_all();
    }

private:
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    int readers_{0};
    bool writer_active_{false};
    bool writer_waiting_{false};
};
