#include <iostream>
#include <thread>
#include <unistd.h>
#include <mutex>

using namespace std;

mutex mtx;     // Global taala — sabke liye shared
mutex mtx_try; // try_lock ke liye alag mutex

// ============================================================
// TASK A — Raw mutex: manual lock() / unlock()
// ============================================================
void taskA()
{
    for (int i = 0; i < 5; i++)
    {
        sleep(1);
        mtx.lock(); // Haath se taala lagaya
        cout << "[TaskA - raw mutex    ] i = " << i << endl;
        mtx.unlock(); // Haath se taala khola
        // ⚠️ DANGER: Agar yahan exception aaye toh unlock kabhi nahi hoga
        //            aur program DEADLOCK mein phans jayega!
    }
}

// ============================================================
// TASK B — lock_guard: automatic lock/unlock (RAII)
// ============================================================
void taskB()
{
    for (int i = 0; i < 5; i++)
    {
        sleep(1);
        {
            lock_guard<mutex> lock(mtx); // { shuru hote hi auto-lock
            cout << "[TaskB - lock_guard   ] i = " << i << endl;
        } // } khatam hote hi auto-unlock
        // ✅ SAFE: Exception aaye tab bhi unlock guaranteed hai
        // ❌ LIMIT: Beech mein manually unlock NAHI kar sakte
    }
}

// ============================================================
// TASK C — unique_lock: flexible manual + auto lock/unlock
// ============================================================
void taskC(int id, string name)
{
    for (int i = 0; i < 5; i++)
    {
        sleep(1);
        unique_lock<mutex> lock(mtx); // Lock liya
        cout << "[TaskC - unique_lock  ] i = " << i
             << " | Thread " << id << " (" << name << ")" << endl;

        lock.unlock(); // Beech mein manually unlock kar sakte hain
        // Yahan bina lock ke koi aur kaam kar sakte ho
        lock.lock(); // Dobara lock le sakte ho
        // Scope end pe auto-unlock bhi hoga
    }
}

// ============================================================
// TASK D — defer_lock: pehle object banao, baad mein lock lo
// ============================================================
void taskD()
{
    for (int i = 0; i < 5; i++)
    {
        sleep(1);
        unique_lock<mutex> lock(mtx, defer_lock);
        // defer_lock matlab: object ban gaya par abhi lock NAHI liya

        // Kuch kaam bina lock ke
        int calc = i * i; // Yeh thread-safe hai (shared resource nahi)

        lock.lock(); // Ab lock lo jab zarurat ho
        cout << "[TaskD - defer_lock   ] i = " << i
             << " | i*i = " << calc << endl;
        // Scope end pe auto-unlock
    }
}

// ============================================================
// TASK E — try_lock: block mat karo, check karo milega ya nahi
// ============================================================
void taskE(int id)
{
    for (int i = 0; i < 5; i++)
    {
        sleep(1);
        if (mtx_try.try_lock())
        {
            // Lock mil gaya — kaam karo
            cout << "[TaskE - try_lock     ] Thread " << id
                 << " ne lock liya! i = " << i << endl;
            sleep(1); // Kuch kaam simulate kar rahe hain
            mtx_try.unlock();
        }
        else
        {
            // Lock nahi mila — ruke nahi, aage badh gaye
            cout << "[TaskE - try_lock     ] Thread " << id
                 << " ko lock nahi mila, skip kar raha hoon i = " << i << endl;
        }
    }
}

// ============================================================
// TASK F — std::lock() : Deadlock-safe do mutex ek saath lock karna
// ============================================================
mutex mtxF1, mtxF2;

void taskF1()
{
    for (int i = 0; i < 3; i++)
    {
        sleep(1);
        unique_lock<mutex> l1(mtxF1, defer_lock);
        unique_lock<mutex> l2(mtxF2, defer_lock);
        std::lock(l1, l2); // Dono atomically lock — DEADLOCK impossible
        cout << "[TaskF1 - std::lock   ] Dono locks safely liye, i = " << i << endl;
    }
}

void taskF2()
{
    for (int i = 0; i < 3; i++)
    {
        sleep(1);
        unique_lock<mutex> l1(mtxF1, defer_lock);
        unique_lock<mutex> l2(mtxF2, defer_lock);
        std::lock(l1, l2); // Same order, same atomic call — safe hai
        cout << "[TaskF2 - std::lock   ] Dono locks safely liye, i = " << i << endl;
    }
}

// ============================================================
// MAIN
// ============================================================
int main()
{
    cout << "====== LOCKING MECHANISMS DEMO ======\n"
         << endl;

    cout << "--- 1. Raw Mutex vs lock_guard ---" << endl;
    thread t1(taskA);
    thread t2(taskB);
    t1.join();
    t2.join();

    cout << "\n--- 2. unique_lock (flexible) ---" << endl;
    thread t3(taskC, 1, "Shubham");
    t3.join();

    cout << "\n--- 3. defer_lock ---" << endl;
    thread t4(taskD);
    t4.join();

    cout << "\n--- 4. try_lock (non-blocking) ---" << endl;
    thread t5(taskE, 1);
    thread t6(taskE, 2);
    t5.join();
    t6.join();

    cout << "\n--- 5. std::lock() — Deadlock Fix ---" << endl;
    thread t7(taskF1);
    thread t8(taskF2);
    t7.join();
    t8.join();

    cout << "\nMera PC ek saath "
         << thread::hardware_concurrency()
         << " threads chala sakta hai!" << endl;

    return 0;
}

/*
    COMPLETE LOCKING MECHANISMS — Hinglish Explanation
    ====================================================

    1. RAW MUTEX — mtx.lock() / mtx.unlock()
       - Seedha mutex ko manually lock aur unlock karte hain.
       - PROBLEM: Agar lock ke baad exception aaye → unlock kabhi nahi hoga
                  → Program hamesha ke liye DEADLOCK mein phans jayega.
       - Isliye production code mein raw mutex avoid karo.

    2. lock_guard — Automatic RAII Lock
       - { bracket shuru hote hi lock, } khatam hote hi auto-unlock.
       - Exception safe — scope khatam = guaranteed unlock.
       - LIMITATION: Beech mein unlock NAHI kar sakte.
       - Best for: Simple critical sections.

    3. unique_lock — Flexible Lock
       - lock(), unlock(), try_lock() — sab manually call kar sakte ho.
       - Condition variables ke saath SIRF yahi kaam karta hai.
       - lock_guard se thoda slow — extra state store karta hai.
       - Best for: Complex locking logic, condition variables.

    4. defer_lock — Lazy Locking
       - unique_lock<mutex> lock(mtx, defer_lock);
         → Object ban gaya, par ABHI lock nahi liya.
       - Baad mein lock.lock() se manually lo.
       - Use case: Pehle kuch setup karo, phir lock lo.
       - std::lock() ke saath use hota hai deadlock prevent karne ke liye.

    5. try_lock — Non-Blocking Attempt
       - Lock milega toh true, nahi milega toh false — TURANT.
       - Thread rukti NAHI hai — chahe lock mile ya na mile.
       - Use case: Jab lock ke bina bhi kaam chal sake.

    6. std::lock() — Deadlock-Safe Multi-Lock
       - DEADLOCK tab hota hai:
           T1: mtxA lock kiya, mtxB ka wait kar raha hai
           T2: mtxB lock kiya, mtxA ka wait kar raha hai
           → Dono forever rukein → DEADLOCK
       - std::lock(l1, l2) dono ko ATOMICALLY lock karta hai.
         Matlab ya toh dono milenge, ya koi nahi — beech ki state nahi.
       - defer_lock ke saath use karo taaki pehle objects bano,
         phir std::lock() se ek saath lo.

    COMPARISON TABLE:
    -----------------------------------------------------------------
    Feature           | raw mutex | lock_guard | unique_lock
    -----------------------------------------------------------------
    Auto unlock       |    NO     |    YES     |    YES
    Manual unlock     |    YES    |    NO      |    YES
    Re-lock           |    YES    |    NO      |    YES
    defer_lock        |    N/A    |    NO      |    YES
    try_lock          |    YES    |    NO      |    YES
    Condition var     |    NO     |    NO      |    YES
    Exception safe    |    NO     |    YES     |    YES
    Performance       |   Fast    |   Fast     | Slight overhead
    -----------------------------------------------------------------

    GOLDEN RULES:
    ✅ Simple lock chahiye           → lock_guard
    ✅ Beech mein unlock chahiye     → unique_lock
    ✅ Condition variable chahiye    → unique_lock (mandatory)
    ✅ 2 mutex ek saath lock karne   → defer_lock + std::lock()
    ✅ Non-blocking check            → try_lock
    ❌ Raw mutex                     → Avoid karo production mein!
*/