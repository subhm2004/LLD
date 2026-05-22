#ifndef MULTI_THREADING_BARRIER_H
#define MULTI_THREADING_BARRIER_H

#include <condition_variable>
#include <mutex>

namespace barrier_sync {

/**
 * Cyclic barrier: N threads wait; when all arrive, everyone proceeds.
 * (Interview alternative to pthread_barrier / C++20 std::barrier.)
 */
class Barrier {
public:
    explicit Barrier(int parties) : parties_(parties), waiting_(0), generation_(0) {}

    void arrive_and_wait() {
        std::unique_lock<std::mutex> lock(mtx_);
        int gen = generation_;
        if (++waiting_ == parties_) {
            waiting_ = 0;
            ++generation_;
            cv_.notify_all();
            return;
        }
        cv_.wait(lock, [this, gen] { return generation_ != gen; });
    }

private:
    int parties_;
    int waiting_;
    int generation_;
    std::mutex mtx_;
    std::condition_variable cv_;
};

}  // namespace barrier_sync

#endif  // MULTI_THREADING_BARRIER_H
