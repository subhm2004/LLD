#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <atomic>
#include <random>

using namespace std;

// ============================================================
// DINING PHILOSOPHERS — Classic Concurrency Problem
// ============================================================
/*
    PROBLEM:
    - N philosophers baithte hain gol table par.
    - Har philosopher ko khane ke liye LEFT + RIGHT fork chahiye.
    - Table par sirf N forks hain — har fork ke beech ek philosopher.
    - Philosopher THINK karta hai, phir EAT, phir repeat.

    CHALLENGE:
    - Sab ek saath left fork uthaye → DEADLOCK (sab right fork ka wait)
    - Solution: resource ordering, waiter (semaphore), ya try-lock backoff
*/

const int NUM_PHILOSOPHERS = 5;
const int EAT_ROUNDS = 2;

atomic<bool> running{true};

void think(int id)
{
    this_thread::sleep_for(chrono::milliseconds(50 + (id * 17) % 80));
    cout << "[P" << id << "] Thinking..." << endl;
}

void eat(int id)
{
    cout << "[P" << id << "] *** EATING ***" << endl;
    this_thread::sleep_for(chrono::milliseconds(100));
}

// ============================================================
// VERSION 1 — NAIVE (DEADLOCK PRONE) — Demo only, short timeout
// ============================================================
/*
    ⚠️ Yeh version deadlock kar sakta hai!
    Neeche main() mein isko default OFF rakha hai.
*/
vector<mutex> forks_v1(NUM_PHILOSOPHERS);

void philosopherNaive(int id)
{
  for (int round = 0; round < EAT_ROUNDS && running; ++round)
    {
        think(id);

        int left = id;
        int right = (id + 1) % NUM_PHILOSOPHERS;

        forks_v1[left].lock();
        cout << "[Naive P" << id << "] picked LEFT fork " << left << endl;
        this_thread::sleep_for(chrono::milliseconds(10)); // deadlock chance badhao

        forks_v1[right].lock();
        cout << "[Naive P" << id << "] picked RIGHT fork " << right << endl;

        eat(id);

        forks_v1[right].unlock();
        forks_v1[left].unlock();
    }
}

void runNaiveDemo()
{
    cout << "\n=== VERSION 1: NAIVE (may deadlock — 2s watch) ===\n";
    running = true;
    vector<thread> threads;
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i)
    {
        threads.emplace_back(philosopherNaive, i);
    }

    this_thread::sleep_for(chrono::seconds(2));
    running = false;

    for (auto &t : threads)
    {
        if (t.joinable())
        {
            t.detach(); // avoid hang if deadlocked
        }
    }
    cout << "(Naive demo stopped — deadlock possible)\n";
}

// ============================================================
// VERSION 2 — RESOURCE HIERARCHY (Ordered Fork Locking)
// ============================================================
/*
    FIX: Hamesha pehle chhoti index wali fork, phir badi.
    Circular wait break hoti hai — DEADLOCK nahi.
*/
vector<mutex> forks_v2(NUM_PHILOSOPHERS);

void philosopherOrdered(int id)
{
    for (int round = 0; round < EAT_ROUNDS; ++round)
    {
        think(id);

        int left = id;
        int right = (id + 1) % NUM_PHILOSOPHERS;
        int first = min(left, right);
        int second = max(left, right);

        forks_v2[first].lock();
        forks_v2[second].lock();

        eat(id);

        forks_v2[second].unlock();
        forks_v2[first].unlock();
    }
    cout << "[Ordered P" << id << "] finished all rounds." << endl;
}

void runOrderedDemo()
{
    cout << "\n=== VERSION 2: ORDERED FORKS (deadlock-free) ===\n";
    vector<thread> threads;
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i)
    {
        threads.emplace_back(philosopherOrdered, i);
    }
    for (auto &t : threads)
    {
        t.join();
    }
}

// ============================================================
// VERSION 3 — WAITER / ARBITRATOR (Semaphore N-1)
// ============================================================
/*
    FIX: Ek waiter sirf (N-1) philosophers ko table par baithne deta hai.
    Agar sab N hi baithe aur sab left fork uthaye → deadlock.
    N-1 se kam se kam ek ke paas dono fork nahi honge → safe.
*/
vector<mutex> forks_v3(NUM_PHILOSOPHERS);
mutex waiterMtx;
int waiterCount = NUM_PHILOSOPHERS - 1; // max at table
condition_variable waiterCv;

void acquireTableSeat(int id)
{
    unique_lock<mutex> lock(waiterMtx);
    waiterCv.wait(lock, []
                  { return waiterCount > 0; });
    --waiterCount;
    cout << "[Waiter] P" << id << " allowed at table. Seats left=" << waiterCount << endl;
}

void releaseTableSeat(int id)
{
    {
        lock_guard<mutex> lock(waiterMtx);
        ++waiterCount;
        cout << "[Waiter] P" << id << " left table. Seats left=" << waiterCount << endl;
    }
    waiterCv.notify_one();
}

void philosopherWaiter(int id)
{
    for (int round = 0; round < EAT_ROUNDS; ++round)
    {
        think(id);
        acquireTableSeat(id);

        int left = id;
        int right = (id + 1) % NUM_PHILOSOPHERS;

        forks_v3[left].lock();
        forks_v3[right].lock();

        eat(id);

        forks_v3[right].unlock();
        forks_v3[left].unlock();

        releaseTableSeat(id);
    }
    cout << "[Waiter P" << id << "] finished all rounds." << endl;
}

void runWaiterDemo()
{
    cout << "\n=== VERSION 3: WAITER (semaphore N-1) ===\n";
    waiterCount = NUM_PHILOSOPHERS - 1;
    vector<thread> threads;
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i)
    {
        threads.emplace_back(philosopherWaiter, i);
    }
    for (auto &t : threads)
    {
        t.join();
    }
}

// ============================================================
// VERSION 4 — TRY-LOCK WITH BACKOFF (Livelock-safe retry)
// ============================================================
vector<mutex> forks_v4(NUM_PHILOSOPHERS);

void philosopherTryLock(int id)
{
    mt19937 rng(id + static_cast<unsigned>(chrono::steady_clock::now().time_since_epoch().count()));
    uniform_int_distribution<int> backoffMs(20, 80);

    for (int round = 0; round < EAT_ROUNDS; ++round)
    {
        think(id);

        int left = id;
        int right = (id + 1) % NUM_PHILOSOPHERS;

        while (true)
        {
            if (forks_v4[left].try_lock())
            {
                if (forks_v4[right].try_lock())
                {
                    eat(id);
                    forks_v4[right].unlock();
                    forks_v4[left].unlock();
                    break;
                }
                forks_v4[left].unlock();
            }
            this_thread::sleep_for(chrono::milliseconds(backoffMs(rng)));
        }
    }
    cout << "[TryLock P" << id << "] finished all rounds." << endl;
}

void runTryLockDemo()
{
    cout << "\n=== VERSION 4: TRY-LOCK + BACKOFF ===\n";
    vector<thread> threads;
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i)
    {
        threads.emplace_back(philosopherTryLock, i);
    }
    for (auto &t : threads)
    {
        t.join();
    }
}

// ============================================================
// MAIN
// ============================================================
int main()
{
    cout << "############################################\n";
    cout << "#   DINING PHILOSOPHERS — Concurrency Demo  #\n";
    cout << "############################################\n";

    // Uncomment to see deadlock risk (may hang — Ctrl+C to stop):
    // runNaiveDemo();

    runOrderedDemo();
    runWaiterDemo();
    runTryLockDemo();

    cout << "\nAll safe demos completed successfully.\n";
    return 0;
}

/*
    DINING PHILOSOPHERS — Hinglish Summary
    ======================================

    KYUN FAMOUS HAI:
    - Deadlock ka classic example (circular wait on forks)
    - Interview mein aksar puchte: "5 philosophers design karo"

    SOLUTIONS IS FILE MEIN:

    1. NAIVE (left then right, no ordering)
       → Sab left fork le lein → deadlock possible

    2. ORDERED FORKS (resource hierarchy)
       → Pehle min(index), phir max(index)
       → Circular wait break — simple & effective

    3. WAITER (at most N-1 philosophers eat)
       → condition_variable + counter
       → Kam se kam ek philosopher ko dono fork nahi milenge

    4. TRY-LOCK + BACKOFF
       → Fork na mile toh chhod do, random wait, retry
       → Deadlock avoid, starvation possible (rare with backoff)

    COMPILE:
    g++ -std=c++17 -pthread dining_philosophers.cpp -o dining_philosophers && ./dining_philosophers

    LINK TO REPO:
    - deadlock_and_protection.cpp → general deadlock fixes
    - semaphor.cpp → waiter idea similar to counting semaphore
    - LRU_Cache_LLD → real project using mutex
*/
