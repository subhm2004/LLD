#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
using namespace std;

// ============================================================
// SHARED RESOURCES
// ============================================================
int unsafeCounter = 0;        // Koi protection nahi
int mutexCounter = 0;         // Mutex se protected
atomic<int> atomicCounter(0); // Atomic se protected
mutex mtx;

// ============================================================
// 1. UNSAFE — Race Condition (Koi lock nahi)
// ============================================================
void incrementUnsafe()
{
    for (int i = 0; i < 100000; i++)
    {
        unsafeCounter++; // ⚠️ Race condition yahan hai!
        /*
            CPU ke liye yeh 3 alag steps hain:
            Step 1: Memory se value LOAD karo  (read)
            Step 2: Value ko +1 karo            (modify)
            Step 3: Memory mein wapas STORE karo (write)

            Dono threads ek saath Step 1 kar sakti hain
            → Dono purani value padhti hain
            → Dono +1 karti hain
            → Dono same nai value likhti hain
            → Net effect: sirf +1 hua, +2 nahi!
        */
    }
}

// ============================================================
// 2. MUTEX — Safe Counter
// ============================================================
void incrementMutex()
{
    for (int i = 0; i < 100000; i++)
    {
        lock_guard<mutex> lock(mtx); // Ek waqt sirf ek thread
        mutexCounter++;              // Ab safe hai
    }
}

// ============================================================
// 3. ATOMIC — Fastest Safe Counter
// ============================================================
void incrementAtomic()
{
    for (int i = 0; i < 100000; i++)
    {
        atomicCounter++; // Hardware level pe atomic — no lock needed
    }
}

// ============================================================
// 4. BANK ACCOUNT — Real World Race Condition Example
// ============================================================
int balance = 10000; // Shared bank balance
mutex bankMtx;

void withdrawUnsafe(int amount, string user)
{
    if (balance >= amount) // Step 1: Check karo
    {
        // ⚠️ Yahan dono threads ek saath pahunch sakti hain
        // Dono check pass kar leti hain — dono withdraw kar leti hain
        // Balance negative ho jaata hai!
        this_thread::sleep_for(chrono::milliseconds(1)); // Context switch simulate
        balance -= amount;                               // Step 2: Withdraw karo
        cout << "[UNSAFE] " << user << " ne " << amount
             << " nikale | Bacha: " << balance << endl;
    }
    else
    {
        cout << "[UNSAFE] " << user << " — Insufficient balance!" << endl;
    }
}

void withdrawSafe(int amount, string user)
{
    lock_guard<mutex> lock(bankMtx); // Pehle lock lo
    if (balance >= amount)
    {
        balance -= amount;
        cout << "[SAFE]   " << user << " ne " << amount
             << " nikale | Bacha: " << balance << endl;
    }
    else
    {
        cout << "[SAFE]   " << user << " — Insufficient balance!" << endl;
    }
}

// ============================================================
// 5. PRINT ORDER — Output Synchronization
// ============================================================
mutex printMtx;

void printUnsync(string name, int count)
{
    for (int i = 1; i <= count; i++)
        cout << "[" << name << "] " << i << endl; // Mixed output aayega
}

void printSync(string name, int count)
{
    for (int i = 1; i <= count; i++)
    {
        lock_guard<mutex> lock(printMtx);
        cout << "[" << name << "] " << i << endl; // Clean output
    }
}

// ============================================================
// MAIN
// ============================================================
int main()
{
    // ----------------------------------------------------------
    // TEST 1: Race Condition vs Mutex vs Atomic
    // ----------------------------------------------------------
    cout << "====== TEST 1: Counter Race Condition ======" << endl;
    cout << "Expected value: 200000 (2 threads × 100000)\n"
         << endl;

    // Unsafe
    unsafeCounter = 0;
    {
        thread t1(incrementUnsafe);
        thread t2(incrementUnsafe);
        t1.join();
        t2.join();
    }
    cout << "Unsafe Counter  : " << unsafeCounter
         << (unsafeCounter == 200000 ? " ✅" : " ❌ (Race condition!)") << endl;

    // Mutex
    mutexCounter = 0;
    {
        thread t1(incrementMutex);
        thread t2(incrementMutex);
        t1.join();
        t2.join();
    }
    cout << "Mutex Counter   : " << mutexCounter
         << (mutexCounter == 200000 ? " ✅" : " ❌") << endl;

    // Atomic
    atomicCounter = 0;
    {
        thread t1(incrementAtomic);
        thread t2(incrementAtomic);
        t1.join();
        t2.join();
    }
    cout << "Atomic Counter  : " << atomicCounter
         << (atomicCounter == 200000 ? " ✅" : " ❌") << endl;

    // ----------------------------------------------------------
    // TEST 2: Bank Account Race Condition
    // ----------------------------------------------------------
    cout << "\n====== TEST 2: Bank Account ======" << endl;

    // Unsafe withdrawal
    balance = 10000;
    cout << "Starting Balance: " << balance << endl;
    cout << "Dono log 8000 nikalna chahte hain:\n"
         << endl;
    {
        thread t1(withdrawUnsafe, 8000, "Alice");
        thread t2(withdrawUnsafe, 8000, "Bob  ");
        t1.join();
        t2.join();
    }
    cout << "Final Balance (Unsafe): " << balance
         << (balance < 0 ? " ❌ NEGATIVE! Race condition!" : " ✅") << endl;

    // Safe withdrawal
    balance = 10000;
    cout << "\nStarting Balance: " << balance << endl;
    cout << "Dono log 8000 nikalna chahte hain:\n"
         << endl;
    {
        thread t1(withdrawSafe, 8000, "Alice");
        thread t2(withdrawSafe, 8000, "Bob  ");
        t1.join();
        t2.join();
    }
    cout << "Final Balance (Safe)  : " << balance
         << (balance >= 0 ? " ✅" : " ❌") << endl;

    // ----------------------------------------------------------
    // TEST 3: Output Synchronization
    // ----------------------------------------------------------
    cout << "\n====== TEST 3: Print Synchronization ======" << endl;

    cout << "\n--- Unsynchronized (Mixed output) ---" << endl;
    {
        thread t1(printUnsync, "Hello", 4);
        thread t2(printUnsync, "Bye  ", 4);
        t1.join();
        t2.join();
    }

    cout << "\n--- Synchronized (Clean output) ---" << endl;
    {
        thread t1(printSync, "Hello", 4);
        thread t2(printSync, "Bye  ", 4);
        t1.join();
        t2.join();
    }

    return 0;
}

/*
    RACE CONDITION & SYNCHRONIZATION — Detailed Hinglish Explanation
    =================================================================

    -------------------------------------------------------
    RACE CONDITION KYA HOTI HAI
    -------------------------------------------------------

    Jab 2 ya zyada threads ek hi shared variable ko
    SAME WAQT pe read/write karti hain bina kisi control ke
    toh result unpredictable ho jaata hai.
    Isse Race Condition kehte hain.

    Naam kyun "Race"?
    Kyunki threads ek doosre se "race" karti hain
    shared resource access karne ke liye —
    jo pehle pahuncha usne apna kaam kiya,
    doosre ko pata bhi nahi chala.

    -------------------------------------------------------
    COUNTER++ KA ASLI KAAM — 3 CPU Steps
    -------------------------------------------------------

    counter++; ek simple line dikhti hai par CPU ke liye 3 steps hain:

    Step 1 — LOAD   : Memory se counter ki value registers mein load karo
    Step 2 — ADD    : Register mein +1 karo
    Step 3 — STORE  : Nayi value wapas memory mein likho

    Race Condition tab hoti hai:

    Time  | Thread 1              | Thread 2
    ------|----------------------|----------------------
    T1    | LOAD counter (= 5)   |
    T2    |                      | LOAD counter (= 5)
    T3    | ADD → 6              |
    T4    |                      | ADD → 6
    T5    | STORE 6              |
    T6    |                      | STORE 6  ← Same value!

    Result: counter = 6  (Expected: 7)
    Ek increment lost ho gaya — yahi Race Condition hai!

    -------------------------------------------------------
    TEEN SOLUTIONS AUR UNKA COMPARISON
    -------------------------------------------------------

    SOLUTION 1 — Raw (No Protection):
    counter++;
    ❌ Thread safe nahi
    ❌ Result unpredictable
    ✅ Sabse fast

    SOLUTION 2 — Mutex + lock_guard:
    lock_guard<mutex> lock(mtx);
    counter++;
    ✅ 100% thread safe
    ✅ Exception safe (RAII)
    ⚠️ Slow — har iteration pe OS-level lock/unlock
    ⚠️ Ek waqt sirf ek thread — baaki wait karti hain

    SOLUTION 3 — atomic<int>:
    atomic<int> counter(0);
    counter++;
    ✅ 100% thread safe
    ✅ Mutex se FAST — CPU instruction level pe atomic
    ✅ No lock needed — hardware guarantee
    ⚠️ Sirf simple operations ke liye (++, --, +=)
    ⚠️ Complex critical sections ke liye mutex hi use karo

    -------------------------------------------------------
    BANK ACCOUNT EXAMPLE — Real World Race Condition
    -------------------------------------------------------

    Balance = 10000
    Alice aur Bob dono 8000 nikalna chahte hain.

    UNSAFE (Bina Lock):
    Time | Alice                    | Bob
    -----|--------------------------|---------------------------
    T1   | balance check: 10000≥8000 ✅ |
    T2   |                          | balance check: 10000≥8000 ✅
    T3   | balance -= 8000 → 2000   |
    T4   |                          | balance -= 8000 → -6000 ❌

    Dono ka check pass ho gaya kyunki dono ne
    purana balance (10000) dekha.
    Result: balance = -6000 ← NEGATIVE! Bank loss!

    SAFE (Mutex ke saath):
    Alice pehle lock leti hai → check → withdraw → unlock
    Bob lock lene ki koshish karta hai → wait...
    Alice khatam → Bob ka lock milta hai → check: 2000 < 8000
    Bob → "Insufficient balance!" ✅
    Result: balance = 2000 ← Correct!

    -------------------------------------------------------
    OUTPUT SYNCHRONIZATION — cout ka Race
    -------------------------------------------------------

    cout bhi ek shared resource hai.

    Bina sync ke:
    Thread1 likhna shuru karta hai: "[Hello] 1"
    Beech mein Thread2 ghus jaata hai: "[Bye  ] 1"
    Output: "[Hello[Bye  ] 1] 1" ← Garbled!

    Mutex ke saath:
    Thread1 ne lock liya → poora cout likha → unlock
    Thread2 ne lock liya → poora cout likha → unlock
    Output: Clean aur readable ✅

    -------------------------------------------------------
    ATOMIC KYU FAST HAI MUTEX SE
    -------------------------------------------------------

    Mutex (OS Level):
    lock()   → OS ko call karo → Thread ko block karo
    unlock() → OS ko call karo → Doosri thread ko jagao
    Yeh sab karne mein microseconds lagte hain.

    Atomic (Hardware Level):
    CPU ek special instruction use karta hai: CAS
    CAS = Compare And Swap
    Ek single unbreakable CPU instruction mein:
    → Purani value check karo
    → Agar same hai toh nayi value set karo
    → Sab ek hi clock cycle mein!

    No OS call, No thread blocking, No waiting.
    Isliye atomic << mutex in speed.

    -------------------------------------------------------
    KAB KYA USE KAREIN
    -------------------------------------------------------

    Simple counter ya flag    → atomic<int> / atomic<bool>
    Complex critical section  → mutex + lock_guard
    Beech mein unlock chahiye → mutex + unique_lock
    cout protect karna        → mutex + lock_guard
    Maximum performance       → atomic (jahan possible ho)

    -------------------------------------------------------
    SYNCHRONIZATION KE 3 GOLDEN RULES
    -------------------------------------------------------

    Rule 1 — Shared data ko hamesha protect karo:
    Agar ek thread likhti hai aur doosri padhti hai
    toh mutex ya atomic ZAROOR lagao.

    Rule 2 — Lock ka scope chhota rakho:
    Sirf critical section ko lock karo.
    Sleep, I/O, ya time-consuming kaam lock ke andar mat karo.
    Jitna kam time lock laga, utna zyada parallel execution.

    Rule 3 — Consistent order mein locks lo:
    Agar 2 mutex hain toh HAMESHA same order mein lock karo.
    Thread1: mtxA → mtxB  ✅
    Thread2: mtxA → mtxB  ✅
    Kabhi bhi opposite order mat karo — DEADLOCK hoga!

    -------------------------------------------------------
    COMPARISON TABLE
    -------------------------------------------------------

    Feature           | Unsafe  | Mutex      | Atomic
    ------------------|---------|------------|----------
    Thread Safe       | NO ❌   | YES ✅     | YES ✅
    Speed             | Fast ⚡ | Slow 🐢    | Fast ⚡
    Complex ops       | YES     | YES        | NO
    Exception safe    | N/A     | YES (RAII) | YES
    OS involvement    | NO      | YES        | NO
    Use case          | Never   | Complex    | Simple
                      | in MT   | sections   | counters
*/