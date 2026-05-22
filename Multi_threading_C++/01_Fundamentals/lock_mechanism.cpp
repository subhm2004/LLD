#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

mutex mtx1, mtx2, mtx3;

// ✅ 1. lock_guard demo
void lockGuardDemo()
{
    lock_guard<mutex> lock(mtx1);
    cout << "[lock_guard] Lock auto-liya (scope enter pe)" << endl;
    this_thread::sleep_for(chrono::milliseconds(100));
    cout << "[lock_guard] Kaam khatam, scope end pe auto-unlock hoga" << endl;
    // } yahan scope khatam → automatic unlock, koi choice nahi
}

// ✅ 2. unique_lock demo
void uniqueLockDemo()
{
    unique_lock<mutex> lock(mtx2);
    cout << "[unique_lock] Lock liya" << endl;
    this_thread::sleep_for(chrono::milliseconds(100));

    lock.unlock();
    cout << "[unique_lock] Manually unlock kar diya" << endl;

    this_thread::sleep_for(chrono::milliseconds(50));

    lock.lock();
    cout << "[unique_lock] Dobara lock liya" << endl;
    // Scope end pe auto-unlock bhi hoga
}

// ✅ 3. Raw mutex demo (manual lock/unlock)
void rawMutexDemo()
{
    mtx3.lock();
    cout << "[raw mutex] Manually lock kiya" << endl;
    this_thread::sleep_for(chrono::milliseconds(100));
    cout << "[raw mutex] Manually unlock kar rahe hain" << endl;
    mtx3.unlock();
    // ⚠️ Agar yahan exception aaye toh unlock nahi hoga → DEADLOCK!
}

// ❌ 4. Deadlock demo (DO NOT run in production!)
mutex dmtx1, dmtx2;
void deadlockThread1()
{
    lock_guard<mutex> l1(dmtx1);
    this_thread::sleep_for(chrono::milliseconds(50));
    lock_guard<mutex> l2(dmtx2); // dmtx2 ka wait karega
    cout << "[Deadlock T1] Dono locks mil gaye" << endl;
}
void deadlockThread2()
{
    lock_guard<mutex> l1(dmtx2);
    this_thread::sleep_for(chrono::milliseconds(50));
    lock_guard<mutex> l2(dmtx1); // dmtx1 ka wait karega → DEADLOCK
    cout << "[Deadlock T2] Dono locks mil gaye" << endl;
}

// ✅ 5. Deadlock FIX — std::lock() se
mutex smtx1, smtx2;
void safeThread1()
{
    unique_lock<mutex> l1(smtx1, defer_lock); // abhi lock mat lo
    unique_lock<mutex> l2(smtx2, defer_lock); // abhi lock mat lo
    std::lock(l1, l2);                        // Dono ko ATOMICALLY lock karo — deadlock impossible
    cout << "[Safe T1] Dono locks safely liye" << endl;
}
void safeThread2()
{
    unique_lock<mutex> l1(smtx1, defer_lock);
    unique_lock<mutex> l2(smtx2, defer_lock);
    std::lock(l1, l2);
    cout << "[Safe T2] Dono locks safely liye" << endl;
}

// ✅ 6. try_lock demo
mutex tmtx;
void tryLockDemo(int id)
{
    if (tmtx.try_lock())
    { // Block nahi karta — turant true/false deta hai
        cout << "[try_lock] Thread " << id << " ne lock liya!" << endl;
        this_thread::sleep_for(chrono::milliseconds(200));
        tmtx.unlock();
        cout << "[try_lock] Thread " << id << " ne unlock kiya" << endl;
    }
    else
    {
        cout << "[try_lock] Thread " << id << " ko lock nahi mila, aage badh gaya" << endl;
    }
}

int main()
{
    cout << "===== 1. lock_guard =====" << endl;
    thread t1(lockGuardDemo);
    t1.join();

    cout << "\n===== 2. unique_lock =====" << endl;
    thread t2(uniqueLockDemo);
    t2.join();

    cout << "\n===== 3. Raw Mutex (manual lock/unlock) =====" << endl;
    thread t3(rawMutexDemo);
    t3.join();

    cout << "\n===== 4. Deadlock FIX (std::lock) =====" << endl;
    thread t4(safeThread1);
    thread t5(safeThread2);
    t4.join();
    t5.join();

    cout << "\n===== 5. try_lock =====" << endl;
    thread t6(tryLockDemo, 1);
    thread t7(tryLockDemo, 2);
    t6.join();
    t7.join();

    return 0;
}

/*
    LOCKING MECHANISMS — Complete Explanation (Hinglish)
    =====================================================

    1. lock_guard:
       - Sabse simple wrapper hai mutex ka.
       - Constructor mein lock leta hai, destructor mein unlock.
       - Manual unlock POSSIBLE NAHI — scope khatam = unlock.
       - Exception safe hai — scope exit pe guaranteed unlock.
       - Use karo: Simple critical sections ke liye.

    2. unique_lock:
       - Flexible wrapper — lock_guard ka powerful bhai.
       - lock(), unlock(), try_lock() — sab manually call kar sakte ho.
       - defer_lock flag se bina lock liye object bana sakte ho.
       - Condition variables ke saath SIRF yahi kaam karta hai.
       - Use karo: Jab beech mein unlock karna ho ya cond_var chahiye.

    3. Raw mutex (mtx.lock() / mtx.unlock()):
       - Direct hardware-level locking.
       - DANGEROUS — agar exception aaye ya unlock bhool jao → DEADLOCK.
       - Koi RAII protection nahi.
       - Use karo: Almost KABHI NAHI — prefer lock_guard/unique_lock.

    4. Deadlock:
       - Tab hota hai jab:
           Thread1 → Lock A liya, Lock B ka wait kar raha hai
           Thread2 → Lock B liya, Lock A ka wait kar raha hai
           → Dono forever wait karte rahenge!
       - FIX: std::lock(l1, l2) — dono ko ek saath atomically lock karo.
         Ya hamesha same ORDER mein locks lo.

    5. try_lock():
       - Non-blocking lock attempt.
       - Lock mila → true return karta hai.
       - Lock nahi mila (koi aur use kar raha) → false return karta hai.
       - Thread rukti nahi — turant result milta hai.
       - Use karo: Jab lock ke bina bhi kuch kaam ho sake.

    COMPARISON TABLE:
    ---------------------------------------------------------------
    Feature            | lock_guard | unique_lock | raw mutex
    ---------------------------------------------------------------
    Auto unlock        |    YES     |    YES      |    NO
    Manual unlock      |    NO      |    YES      |    YES
    Re-lock            |    NO      |    YES      |    YES
    defer_lock support |    NO      |    YES      |    N/A
    try_lock support   |    NO      |    YES      |    YES
    With cond_var      |    NO      |    YES      |    NO
    Exception safe     |    YES     |    YES      |    NO
    Performance        |   Fast     | Slightly    |   Fast
                       |            |   slow      |
    ---------------------------------------------------------------

    GOLDEN RULE:
    - Simple lock chahiye       → lock_guard
    - Flexible lock chahiye     → unique_lock
    - Condition variable chahiye → unique_lock (mandatory)
    - Simple counter/flag       → atomic<int> (no lock needed!)
    - Raw mutex                 → Avoid karo!
*/