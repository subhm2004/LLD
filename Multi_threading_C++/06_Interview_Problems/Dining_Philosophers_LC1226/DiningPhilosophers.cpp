#include "DiningPhilosophers.h"

#include <chrono>
#include <random>
#include <thread>

namespace dining_philosophers {

void DiningPhilosophers::wantsToEatTryLock(
    int philosopher, std::function<void()> pickLeftFork,
    std::function<void()> pickRightFork, std::function<void()> eat,
    std::function<void()> putLeftFork, std::function<void()> putRightFork) {
    int left = philosopher;
    int right = (philosopher + 1) % kNumPhilosophers;

    std::mt19937 rng(static_cast<unsigned>(philosopher) +
                     static_cast<unsigned>(
                         std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> backoffMs(1, 10);

    while (true) {
        if (forks_[left].try_lock()) {
            if (forks_[right].try_lock()) {
                pickLeftFork();
                pickRightFork();
                eat();
                putRightFork();
                forks_[right].unlock();
                putLeftFork();
                forks_[left].unlock();
                return;
            }
            forks_[left].unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(backoffMs(rng)));
    }
}

}  // namespace dining_philosophers
