#ifndef MULTI_THREADING_BUILDING_H2O_H
#define MULTI_THREADING_BUILDING_H2O_H

#include <condition_variable>
#include <functional>
#include <mutex>

namespace building_h2o {

/**
 * LeetCode 1117 — 2 hydrogen + 1 oxygen per molecule.
 * Third thread to finish bonding resets counters (avoids early h-- deadlock).
 */
class BuildingH2O {
public:
    void hydrogen(std::function<void()> releaseHydrogen) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return h_count_ < 2; });
        ++h_count_;
        cv_.wait(lock, [this] { return h_count_ == 2 && o_count_ >= 1; });
        releaseHydrogen();
        if (++bonded_count_ == 3) {
            resetMolecule();
        }
        cv_.notify_all();
    }

    void oxygen(std::function<void()> releaseOxygen) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return o_count_ < 1; });
        ++o_count_;
        cv_.wait(lock, [this] { return h_count_ == 2 && o_count_ >= 1; });
        releaseOxygen();
        if (++bonded_count_ == 3) {
            resetMolecule();
        }
        cv_.notify_all();
    }

private:
    void resetMolecule() {
        h_count_ = 0;
        o_count_ = 0;
        bonded_count_ = 0;
    }

    std::mutex mtx_;
    std::condition_variable cv_;
    int h_count_{0};
    int o_count_{0};
    int bonded_count_{0};
};

}  // namespace building_h2o

#endif  // MULTI_THREADING_BUILDING_H2O_H
