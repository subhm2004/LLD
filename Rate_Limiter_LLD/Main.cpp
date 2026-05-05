#include <bits/stdc++.h>
#include "enums/UserTier.h"
#include "model/User.h"
#include "service/RateLimiterService.h"

using namespace std;
using namespace rate_limiter_lld;

// C++17 manual CyclicBarrier
class Barrier
{
    int total, count;
    mutex mtx;
    condition_variable cv;
    bool released = false;

public:
    Barrier(int n) : total(n), count(n) {}
    void wait()
    {
        unique_lock<mutex> lock(mtx);
        if (--count == 0)
        {
            released = true;
            cv.notify_all();
        }
        else
        {
            cv.wait(lock, [this]
                    { return released; });
        }
    }
};

// C++17 manual CountDownLatch
class Latch
{
    int count;
    mutex mtx;
    condition_variable cv;

public:
    Latch(int n) : count(n) {}
    void count_down()
    {
        unique_lock<mutex> lock(mtx);
        if (--count == 0)
            cv.notify_all();
    }
    void wait()
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]
                { return count == 0; });
    }
};

void checkConcurrency(RateLimiterService &rateLimiterService)
{
    User freeUser1("user1", UserTier::FREE);

    int numThreads = 20;
    Barrier barrier(numThreads);
    Latch latch(numThreads);
    mutex coutMtx; // prevent interleaved cout output

    vector<thread> threads;

    for (int i = 1; i <= numThreads; i++)
    {
        threads.emplace_back([&, i]()
                             {
            barrier.wait(); // All threads wait here until all are ready

            bool allowed = rateLimiterService.allowRequest(freeUser1);

            {
                lock_guard<mutex> lock(coutMtx);
                cout << "Thread-" << this_thread::get_id()
                          << " | Request " << i
                          << " for FreeUser1: " << (allowed ? "ALLOWED" : "BLOCKED")
                          << "\n";
            }

            latch.count_down(); });
    }

    latch.wait(); // wait for all threads to finish

    for (auto &t : threads)
    {
        if (t.joinable())
            t.join();
    }
}

int main()
{
    RateLimiterService rateLimiterService;

    User freeUser("user1", UserTier::FREE);       // 10 req in 60 sec
    User premiumUser("user2", UserTier::PREMIUM); // 100 req in 60 sec

    // Uncomment to test sequential requests:
    cout << "=== Free User Requests ===\n";
    for (int i = 1; i <= 15; i++)
    {
        bool allowed = rateLimiterService.allowRequest(freeUser);
        cout << "Request " << i << " for Free User: "
             << (allowed ? "ALLOWED" : "BLOCKED") << "\n";
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    cout << "\n=== Premium User Requests ===\n";
    for (int i = 1; i <= 120; i++)
    {
        bool allowed = rateLimiterService.allowRequest(premiumUser);
        cout << "Request " << i << " for Premium User: "
             << (allowed ? "ALLOWED" : "BLOCKED") << "\n";
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    checkConcurrency(rateLimiterService);

    return 0;
}
