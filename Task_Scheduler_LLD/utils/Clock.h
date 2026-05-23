#ifndef TASK_SCHEDULER_LLD_UTILS_CLOCK_H
#define TASK_SCHEDULER_LLD_UTILS_CLOCK_H

#include <chrono>

namespace task_scheduler_lld {

class IClock {
public:
    virtual ~IClock() = default;
    virtual long long nowEpochMs() const = 0;
};

class SystemClock : public IClock {
public:
    long long nowEpochMs() const override {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
};

/** Deterministic clock for demos — advance time manually from main. */
class SimulatedClock : public IClock {
public:
    long long nowEpochMs() const override { return currentMs_; }

    void setNow(long long epochMs) { currentMs_ = epochMs; }

    void advanceMs(long long deltaMs) { currentMs_ += deltaMs; }

private:
    long long currentMs_{0};
};

}  // namespace task_scheduler_lld

#endif  // TASK_SCHEDULER_LLD_UTILS_CLOCK_H
