#ifndef MULTI_THREADING_DINING_PHILOSOPHERS_LC1226_H
#define MULTI_THREADING_DINING_PHILOSOPHERS_LC1226_H

#include <algorithm>
#include <array>
#include <functional>
#include <mutex>

#include "../common/CountingSemaphore.h"

namespace dining_philosophers {

/** LeetCode 1226 API shape — five philosophers, five forks. */
class DiningPhilosophers {
public:
    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork) {
        orderedForksEat(philosopher, pickLeftFork, pickRightFork, eat, putLeftFork,
                        putRightFork);
    }

    /** Solution 2: at most 4 philosophers may pick forks (waiter / N-1 seats). */
    void wantsToEatWaiter(int philosopher,
                          std::function<void()> pickLeftFork,
                          std::function<void()> pickRightFork,
                          std::function<void()> eat,
                          std::function<void()> putLeftFork,
                          std::function<void()> putRightFork) {
        table_seats_.acquire();
        int left = philosopher;
        int right = (philosopher + 1) % kNumPhilosophers;

        forks_[left].lock();
        pickLeftFork();
        forks_[right].lock();
        pickRightFork();

        eat();

        putRightFork();
        forks_[right].unlock();
        putLeftFork();
        forks_[left].unlock();

        table_seats_.release();
    }

    /** Solution 3: try_lock both forks with random backoff. */
    void wantsToEatTryLock(int philosopher,
                           std::function<void()> pickLeftFork,
                           std::function<void()> pickRightFork,
                           std::function<void()> eat,
                           std::function<void()> putLeftFork,
                           std::function<void()> putRightFork);

private:
    static constexpr int kNumPhilosophers = 5;

    void orderedForksEat(int philosopher,
                         std::function<void()> pickLeftFork,
                         std::function<void()> pickRightFork,
                         std::function<void()> eat,
                         std::function<void()> putLeftFork,
                         std::function<void()> putRightFork) {
        int left = philosopher;
        int right = (philosopher + 1) % kNumPhilosophers;
        int first = std::min(left, right);
        int second = std::max(left, right);

        forks_[first].lock();
        if (first == left) {
            pickLeftFork();
        } else {
            pickRightFork();
        }

        forks_[second].lock();
        if (second == left) {
            pickLeftFork();
        } else {
            pickRightFork();
        }

        eat();

        putRightFork();
        forks_[second].unlock();
        putLeftFork();
        forks_[first].unlock();
    }

    std::array<std::mutex, kNumPhilosophers> forks_;
    interview_common::CountingSemaphore table_seats_{kNumPhilosophers - 1};
};

}  // namespace dining_philosophers

#endif  // MULTI_THREADING_DINING_PHILOSOPHERS_LC1226_H
