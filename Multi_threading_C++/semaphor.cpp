#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
using namespace std;

/*
    SEMAPHORE KYA HOTA HAI?
    - Mutex sirf 1 thread ko allow karta hai ek waqt mein.
    - Semaphore N threads ko allow karta hai ek waqt mein.
    - Ek "counter" hota hai jo track karta hai kitne threads andar hain.
    - counter > 0 → andar aao, counter--
    - counter == 0 → bahar wait karo
    - kaam khatam → counter++ → doosre ko andar aane do
*/

// ============================================================
// SEMAPHORE CLASS — Custom Implementation (C++11/14/17)
// ============================================================
class Semaphore
{
private:
    int count; // Kitne threads ek saath andar aa sakte hain
    mutex mtx;
    condition_variable cv;

public:
    // count = max allowed threads at a time
    Semaphore(int count) : count(count)
    {
        cout << "[Semaphore] Banaya — Max " << count << " threads ek saath allowed." << endl;
    }

    // Andar aao — agar jagah hai toh, warna wait karo
    void acquire()
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]()
                { return count > 0; }); // Jagah hone tak wait
        count--;                        // Ek slot liya
        cout << "[Semaphore] acquire() — Slot liya. Remaining slots: " << count << endl;
    }

    // Bahar aao — slot wapas karo
    void release()
    {
        unique_lock<mutex> lock(mtx);
        count++; // Slot wapas diya
        cout << "[Semaphore] release() — Slot wapas diya. Remaining slots: " << count << endl;
        cv.notify_one(); // Ek waiting thread ko jagao
    }
};

// ============================================================
// C++20 — Built-in Semaphore (Agar compiler support kare)
// ============================================================
// #include <semaphore>
// counting_semaphore<5> sem(3); // Max 5, initial 3
// binary_semaphore bsem(1);     // Mutex jaisa — sirf 0 ya 1
// sem.acquire();
// sem.release();

// ============================================================
// SHARED RESOURCES
// ============================================================
Semaphore dbSemaphore(3); // Sirf 3 threads DB access kar sakti hain
Semaphore printSem(1);    // Binary semaphore — mutex jaisa
mutex coutMtx;

// ============================================================
// 1. COUNTING SEMAPHORE — Database Connection Pool
// ============================================================
void accessDatabase(int threadId)
{
    {
        lock_guard<mutex> lock(coutMtx);
        cout << "[Thread " << threadId << "] DB connection maang raha hoon..." << endl;
    }

    dbSemaphore.acquire(); // Connection milne tak wait karo

    {
        lock_guard<mutex> lock(coutMtx);
        cout << "[Thread " << threadId << "] ✅ DB connection mila! Kaam kar raha hoon..." << endl;
    }

    this_thread::sleep_for(chrono::milliseconds(threadId * 200)); // DB kaam simulate

    {
        lock_guard<mutex> lock(coutMtx);
        cout << "[Thread " << threadId << "] Kaam khatam, connection release kar raha hoon." << endl;
    }

    dbSemaphore.release(); // Connection wapas karo
}

// ============================================================
// 2. BINARY SEMAPHORE — Mutex ki tarah use karna
// ============================================================
Semaphore binarySem(1); // Sirf 1 thread allowed — mutex jaisa
int sharedData = 0;

void modifyData(int threadId, int value)
{
    {
        lock_guard<mutex> lock(coutMtx);
        cout << "[Thread " << threadId << "] Data modify karne ki koshish..." << endl;
    }

    binarySem.acquire(); // Sirf ek thread andar

    sharedData += value;

    {
        lock_guard<mutex> lock(coutMtx);
        cout << "[Thread " << threadId << "] sharedData = " << sharedData << endl;
    }

    this_thread::sleep_for(chrono::milliseconds(100));

    binarySem.release(); // Doosre ko allow karo
}

// ============================================================
// 3. PRODUCER CONSUMER — Semaphore se
// ============================================================
Semaphore emptySem(5); // 5 empty slots hain buffer mein
Semaphore fullSem(0);  // 0 filled slots hain initially
mutex bufferMtx;
vector<int> buffer;

void producer(int id)
{
    for (int i = 1; i <= 3; i++)
    {
        emptySem.acquire(); // Empty slot milne tak wait karo

        {
            lock_guard<mutex> lock(bufferMtx);
            int item = id * 10 + i;
            buffer.push_back(item);
            lock_guard<mutex> coutLock(coutMtx);
            cout << "[Producer " << id << "] Produce kiya: " << item
                 << " | Buffer size: " << buffer.size() << endl;
        }

        fullSem.release(); // Consumer ko batao — item aa gaya
    }
}

void consumer(int id)
{
    for (int i = 1; i <= 3; i++)
    {
        fullSem.acquire(); // Filled slot milne tak wait karo

        {
            lock_guard<mutex> lock(bufferMtx);
            int item = buffer.front();
            buffer.erase(buffer.begin());
            lock_guard<mutex> coutLock(coutMtx);
            cout << "[Consumer " << id << "] Consume kiya: " << item
                 << " | Buffer size: " << buffer.size() << endl;
        }

        emptySem.release(); // Producer ko batao — slot khali ho gaya
    }
}

// ============================================================
// MAIN
// ============================================================
int main()
{
    // ----------------------------------------------------------
    // TEST 1: Counting Semaphore — DB Connection Pool
    // ----------------------------------------------------------
    cout << "====== TEST 1: Counting Semaphore (DB Pool) ======" << endl;
    cout << "6 threads hain, sirf 3 ek saath DB access kar sakte hain\n"
         << endl;
    {
        vector<thread> threads;
        for (int i = 1; i <= 6; i++)
            threads.push_back(thread(accessDatabase, i));
        for (auto &t : threads)
            t.join();
    }

    // ----------------------------------------------------------
    // TEST 2: Binary Semaphore — Mutex jaisa
    // ----------------------------------------------------------
    cout << "\n====== TEST 2: Binary Semaphore (Mutex jaisa) ======" << endl;
    cout << "4 threads hain, sirf 1 ek waqt mein data modify kar sakta hai\n"
         << endl;
    {
        vector<thread> threads;
        for (int i = 1; i <= 4; i++)
            threads.push_back(thread(modifyData, i, i * 10));
        for (auto &t : threads)
            t.join();
        cout << "Final sharedData: " << sharedData << " (Expected: 100)" << endl;
    }

    // ----------------------------------------------------------
    // TEST 3: Producer Consumer with Semaphore
    // ----------------------------------------------------------
    cout << "\n====== TEST 3: Producer-Consumer (Semaphore se) ======" << endl;
    cout << "Buffer capacity: 5 | 2 Producers, 2 Consumers\n"
         << endl;
    {
        thread p1(producer, 1);
        thread p2(producer, 2);
        thread c1(consumer, 1);
        thread c2(consumer, 2);
        p1.join();
        p2.join();
        c1.join();
        c2.join();
    }

    return 0;
}

/*
    SEMAPHORE — Complete Hinglish Explanation
    ==========================================

    -------------------------------------------------------
    SEMAPHORE KYA HAI — Simple Analogy
    -------------------------------------------------------

    Parking Lot sochlo:
    - 3 parking spots hain (count = 3)
    - Car aayi → spot available? → andar aao (acquire, count--)
    - Spot nahi? → bahar wait karo
    - Car gayi → spot khali (release, count++) → agli car andar

    Yahi Semaphore hai — ek "counter" jo control karta hai
    kitne threads ek saath kisi resource ko use kar sakti hain.

    -------------------------------------------------------
    SEMAPHORE KE 2 TYPES
    -------------------------------------------------------

    1. COUNTING SEMAPHORE:
       - count = N (N > 1)
       - N threads ek saath andar aa sakti hain
       - Use case: DB connection pool, rate limiting,
                   limited resource access

    2. BINARY SEMAPHORE:
       - count = 1 (sirf 0 ya 1)
       - Ek waqt sirf 1 thread andar
       - Mutex jaisa kaam karta hai
       - DIFFERENCE from mutex:
         Mutex: Jo lock kare wahi unlock kare (ownership)
         Binary Sem: Koi bhi release kar sakta hai (no ownership)

    -------------------------------------------------------
    ACQUIRE aur RELEASE — Step by Step
    -------------------------------------------------------

    acquire() — Andar aana:
    Step 1: Lock lo
    Step 2: count > 0 hai? → Nahi toh wait karo
    Step 3: count-- (slot liya)
    Step 4: Lock chhhodo, kaam karo

    release() — Bahar jaana:
    Step 1: Lock lo
    Step 2: count++ (slot wapas diya)
    Step 3: notify_one() — ek waiting thread jagao
    Step 4: Lock chhhodo

    -------------------------------------------------------
    PRODUCER-CONSUMER WITH SEMAPHORE
    -------------------------------------------------------

    2 semaphores use kiye:

    emptySem(5) → Khali jagah track karta hai
    fullSem(0)  → Bhare slots track karta hai

    PRODUCER:
    emptySem.acquire() → Khali slot chahiye
    → Item daalo buffer mein
    fullSem.release()  → Consumer ko batao item aaya

    CONSUMER:
    fullSem.acquire()  → Filled slot chahiye
    → Item nikalo buffer se
    emptySem.release() → Producer ko batao jagah khali hui

    Yeh ensure karta hai:
    ✅ Producer full buffer mein nahi daalta
    ✅ Consumer empty buffer se nahi nikalata
    ✅ Dono synchronized hain

    -------------------------------------------------------
    SEMAPHORE vs MUTEX
    -------------------------------------------------------

    Feature          | Mutex          | Semaphore
    -----------------|----------------|------------------
    Allowed threads  | Sirf 1         | N (configurable)
    Ownership        | YES — jo lock  | NO — koi bhi
                     | kare wahi      | release kar sakta
                     | unlock kare    |
    Use case         | Critical       | Resource pool,
                     | section        | rate limiting
    Binary version   | Same as mutex  | Binary semaphore
    Signaling        | NO             | YES (cross-thread)
    C++20 built-in   | YES (mutex)    | YES (semaphore)

    -------------------------------------------------------
    SEMAPHORE vs CONDITION VARIABLE
    -------------------------------------------------------

    Condition Variable:
    - Ek specific condition ke liye wait karo
    - Predicate check karna padta hai (spurious wakeup)
    - Zyada flexible — complex conditions handle

    Semaphore:
    - Counter-based — N resources available hain
    - Simpler — acquire/release bas
    - Resource counting ke liye perfect

    -------------------------------------------------------
    C++20 BUILT-IN SEMAPHORE
    -------------------------------------------------------

    #include <semaphore>

    // Counting semaphore — max 10, initially 3 available
    counting_semaphore<10> sem(3);

    // Binary semaphore — 0 ya 1
    binary_semaphore bsem(1);

    // Use karna:
    sem.acquire();      // Blocking — wait karo
    sem.try_acquire();  // Non-blocking — true/false
    sem.release();      // Slot wapas karo

    -------------------------------------------------------
    REAL WORLD USE CASES
    -------------------------------------------------------

    1. Database Connection Pool:
       - 100 users, sirf 10 DB connections
       - Semaphore(10) → sirf 10 threads DB access karein

    2. Rate Limiting:
       - API pe sirf 5 requests per second allow
       - Semaphore(5) → extra requests wait karein

    3. Print Spooler:
       - Sirf 2 printers available
       - Semaphore(2) → 2 print jobs simultaneously

    4. Thread Pool Task Limit:
       - Queue mein sirf 100 tasks allowed
       - Semaphore(100) → zyada tasks wait karein

    -------------------------------------------------------
    GOLDEN RULES
    -------------------------------------------------------

    ✅ N resources control karne hain  → Counting Semaphore
    ✅ Ek resource, koi bhi release    → Binary Semaphore
    ✅ Ek resource, same thread unlock → Mutex
    ✅ Complex conditions              → Condition Variable
    ✅ acquire() ke baad release()     → ZAROOR karo (warna deadlock)
    ❌ acquire() bhool gaye release()  → Deadlock!
    ❌ Zyada release() karo            → count limit se upar → Bug!
*/