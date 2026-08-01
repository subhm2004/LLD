#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

// ============================================================
// SHARED MUTEXES
// ============================================================
mutex mtxA, mtxB; // Do taale — A aur B

// ============================================================
// 1. DEADLOCK DEMO — Classic 2 Thread Deadlock
// ============================================================
/*
    ⚠️ WARNING: Yeh code intentionally deadlock karega!
    Run karo toh program hang ho jaayega — Ctrl+C se band karna padega.
    Samajhne ke liye dekho, run mat karo!
*/
void deadlockThread1()
{
    cout << "[T1] mtxA lock karne ki koshish..." << endl;
    mtxA.lock(); // T1 ne mtxA liya
    cout << "[T1] mtxA mila! Ab mtxB ki koshish..." << endl;
    this_thread::sleep_for(chrono::milliseconds(100)); // Context switch simulate
    mtxB.lock();                                       // T1 mtxB ka wait karega — par T2 ne already le liya!
    cout << "[T1] Dono locks mile! (Yeh kabhi print nahi hoga)" << endl;
    mtxB.unlock();
    mtxA.unlock();
}

void deadlockThread2()
{
    cout << "[T2] mtxB lock karne ki koshish..." << endl;
    mtxB.lock(); // T2 ne mtxB liya
    cout << "[T2] mtxB mila! Ab mtxA ki koshish..." << endl;
    this_thread::sleep_for(chrono::milliseconds(100)); // Context switch simulate
    mtxA.lock();                                       // T2 mtxA ka wait karega — par T1 ne already le liya!
    cout << "[T2] Dono locks mile! (Yeh kabhi print nahi hoga)" << endl;
    mtxA.unlock();
    mtxB.unlock();
}

// ============================================================
// 2. FIX 1 — Same Order Locking
// ============================================================
void safeOrderThread1()
{
    mtxA.lock(); // Pehle hamesha A
    this_thread::sleep_for(chrono::milliseconds(100));
    mtxB.lock(); // Phir hamesha B
    cout << "[Fix1 - T1] Dono locks same order se mile!" << endl;
    mtxB.unlock();
    mtxA.unlock();
}

void safeOrderThread2()
{
    mtxA.lock(); // T2 bhi pehle A — same order!
    this_thread::sleep_for(chrono::milliseconds(100));
    mtxB.lock(); // T2 bhi phir B
    cout << "[Fix1 - T2] Dono locks same order se mile!" << endl;
    mtxB.unlock();
    mtxA.unlock();
}

// ============================================================
// 3. FIX 2 — std::lock() Atomic Locking
// ============================================================
void atomicLockThread1()
{
    unique_lock<mutex> l1(mtxA, defer_lock); // Abhi lock mat lo
    unique_lock<mutex> l2(mtxB, defer_lock); // Abhi lock mat lo
    std::lock(l1, l2);                       // Dono ek saath atomically lock
    cout << "[Fix2 - T1] std::lock se dono ek saath mile!" << endl;
} // Auto unlock

void atomicLockThread2()
{
    unique_lock<mutex> l1(mtxA, defer_lock);
    unique_lock<mutex> l2(mtxB, defer_lock);
    std::lock(l1, l2); // Order matter nahi — atomic hai
    cout << "[Fix2 - T2] std::lock se dono ek saath mile!" << endl;
}


// ============================================================
// 5. FIX 4 — try_lock with Timeout (Deadlock Detect & Recover)
// ============================================================
void tryLockThread(int id)
{
    for (int attempt = 1; attempt <= 3; attempt++)
    {
        if (mtxA.try_lock()) // Block mat karo — try karo
        {
            if (mtxB.try_lock()) // Dusra bhi try karo
            {
                cout << "[Fix4 - T" << id << "] Attempt "
                     << attempt << ": Dono locks mile!" << endl;
                this_thread::sleep_for(chrono::milliseconds(50));
                mtxB.unlock();
                mtxA.unlock();
                return; // Kaam ho gaya — exit
            }
            else
            {
                mtxA.unlock(); // mtxB nahi mila toh mtxA bhi chhhod do
                cout << "[Fix4 - T" << id << "] Attempt "
                     << attempt << ": mtxB nahi mila, retry..." << endl;
            }
        }
        else
        {
            cout << "[Fix4 - T" << id << "] Attempt "
                 << attempt << ": mtxA nahi mila, retry..." << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(id * 30)); // Alag delay — livelock se bachao
    }
    cout << "[Fix4 - T" << id << "] 3 attempts failed!" << endl;
}

// ============================================================
// 6. LIVELOCK CONCEPT DEMO — Dono Polite hain par koi aage nahi badhta
// ============================================================
/*
    Livelock: Deadlock jaisa hi hai par threads "active" rehti hain.
    Dono ek doosre ke liye lock chhod dete hain — par koi kaam nahi hota.

    Example (concept — actual livelock intentionally nahi dikhayenge):
    T1: mtxA liya → mtxB try → nahi mila → mtxA chhoda → retry
    T2: mtxB liya → mtxA try → nahi mila → mtxB chhoda → retry
    → Dono baar baar yehi karte rahenge — progress zero!

    Fix: Random ya id-based delay lagao (upar tryLockThread mein kiya hai)
*/
void liveLockConcept(int id)
{
    cout << "[LiveLock Concept - T" << id << "] "
         << "Main ek doosre ke saath 'polite' hoon par koi aage nahi badhta!" << endl;
    this_thread::sleep_for(chrono::milliseconds(id * 50));
    cout << "[LiveLock Concept - T" << id << "] "
         << "Alag delay se livelock se bacha!" << endl;
}

// ============================================================
// MAIN
// ============================================================
int main()
{
    // ----------------------------------------------------------
    // DEADLOCK DEMO — Comment out hai, samajhne ke liye dekho
    // ----------------------------------------------------------
    cout << "====== DEADLOCK DEMO (Commented Out — Hang hoga!) ======" << endl;
    cout << "T1: mtxA → mtxB lena chahta hai" << endl;
    cout << "T2: mtxB → mtxA lena chahta hai" << endl;
    cout << "Result: Dono ek doosre ka wait karte hain → HANG!\n"
         << endl;
    /*
        thread t1(deadlockThread1);   // Uncomment karo toh program hang ho jaayega
        thread t2(deadlockThread2);
        t1.join();
        t2.join();
    */

    // ----------------------------------------------------------
    // FIX 1: Same Order Locking
    // ----------------------------------------------------------
    cout << "====== FIX 1: Same Order Locking ======" << endl;
    {
        thread t1(safeOrderThread1);
        thread t2(safeOrderThread2);
        t1.join();
        t2.join();
    }

    // ----------------------------------------------------------
    // FIX 2: std::lock() Atomic
    // ----------------------------------------------------------
    cout << "\n====== FIX 2: std::lock() Atomic Locking ======" << endl;
    {
        thread t1(atomicLockThread1);
        thread t2(atomicLockThread2);
        t1.join();
        t2.join();
    }

    // ----------------------------------------------------------
    // FIX 4: try_lock with Retry
    // ----------------------------------------------------------
    cout << "\n====== FIX 4: try_lock with Retry ======" << endl;
    {
        thread t1(tryLockThread, 1);
        thread t2(tryLockThread, 2);
        t1.join();
        t2.join();
    }

    // ----------------------------------------------------------
    // LIVELOCK CONCEPT
    // ----------------------------------------------------------
    cout << "\n====== LIVELOCK CONCEPT ======" << endl;
    {
        thread t1(liveLockConcept, 1);
        thread t2(liveLockConcept, 2);
        t1.join();
        t2.join();
    }

    return 0;
}

/*
    DEADLOCK & PROTECTION — Complete Hinglish Explanation
    ======================================================

    -------------------------------------------------------
    DEADLOCK KYA HOTA HAI
    -------------------------------------------------------

    Deadlock ek aisi situation hai jahan 2 ya zyada threads
    ek doosre ka wait karti hain resources ke liye —
    aur koi bhi aage nahi badh sakti.

    Real life example:
    Do log ek tangi gali mein aamne saamne aa gaye.
    Dono ek doosre ke hatne ka wait kar rahe hain.
    Koi nahi hata — dono forever rukhe hain.

    -------------------------------------------------------
    DEADLOCK KI 4 ZARURI CONDITIONS (Coffman Conditions)
    -------------------------------------------------------

    Deadlock tabhi hota hai jab CHARO conditions ek saath hon:

    1. Mutual Exclusion:
       Resource ek waqt sirf ek thread use kar sakti hai.
       (mutex ka basic purpose — yeh toh hoga hi)

    2. Hold and Wait:
       Thread ek resource hold kiye baithe hai
       aur doosre ka wait kar rahi hai.
       (T1 ne mtxA liya, mtxB ka wait kar raha)

    3. No Preemption:
       Koi thread kisi doosri thread se forcefully
       resource nahi cheen sakti.
       (OS mutex forcefully nahi le sakta)

    4. Circular Wait:
       T1 → T2 ka wait, T2 → T1 ka wait
       Ek circle ban jaata hai — koi exit nahi.

    In 4 mein se koi bhi ek tod do → Deadlock impossible!

    -------------------------------------------------------
    IS CODE MEIN DEADLOCK KAISE BANA
    -------------------------------------------------------

    mtxA aur mtxB — do mutex hain.

    Thread1 ka plan: mtxA lo → mtxB lo
    Thread2 ka plan: mtxB lo → mtxA lo  ← OPPOSITE ORDER!

    Timeline:
    T=0 : T1 ne mtxA lock kiya ✅
    T=0 : T2 ne mtxB lock kiya ✅
    T=100: T1 mtxB maangta hai → T2 ke paas hai → WAIT
    T=100: T2 mtxA maangta hai → T1 ke paas hai → WAIT
    T=∞  : Dono wait karte hain → DEADLOCK!

    -------------------------------------------------------
    FIX 1 — SAME ORDER LOCKING
    -------------------------------------------------------

    Sabse simple fix: Hamesha same order mein locks lo.

    Thread1: mtxA → mtxB  ✅
    Thread2: mtxA → mtxB  ✅ (Same order!)

    Timeline:
    T=0  : T1 ne mtxA lock kiya ✅
    T=0  : T2 ne mtxA try kiya → T1 ke paas hai → WAIT
    T=100: T1 ne mtxB lock kiya ✅
    T=150: T1 ne kaam kiya → dono unlock
    T=150: T2 ko mtxA mila → aage badha ✅

    Circular wait condition TOOT gayi → No deadlock!

    PRO TIP: Global rule banao team mein:
    "Hamesha alphabetical ya defined order mein locks lo"

    -------------------------------------------------------
    FIX 2 — std::lock() ATOMIC LOCKING
    -------------------------------------------------------

    unique_lock<mutex> l1(mtxA, defer_lock); // Object banao, lock mat lo
    unique_lock<mutex> l2(mtxB, defer_lock); // Object banao, lock mat lo
    std::lock(l1, l2);                       // Dono ek saath!

    std::lock() internally kya karta hai:
    - Try karta hai dono ko atomically lock karne ki.
    - Agar koi ek nahi mila toh DONO chhod deta hai.
    - Retry karta hai — is baar dusre se start karta hai.
    - Jab tak dono na milein tab tak retry.

    Deadlock impossible kyun:
    - Ya toh dono milenge, ya koi nahi.
    - Aadha-adhoora lock kabhi nahi rehta.
    - Hold-and-wait condition TOOT gayi!

    defer_lock kyun:
    - Pehle unique_lock object banaate hain bina lock ke.
    - std::lock() ko dete hain — woh manage karta hai.
    - Scope end pe RAII se auto-unlock.

    -------------------------------------------------------
    FIX 3 — scoped_lock (C++17) — RECOMMENDED
    -------------------------------------------------------

    scoped_lock lock(mtxA, mtxB);

    Ek line mein sab kuch! Internally std::lock() jaisa kaam karta hai.

    Fayde:
    ✅ Sabse clean syntax — ek line!
    ✅ Deadlock proof — std::lock internally
    ✅ RAII — scope end pe auto-unlock
    ✅ Multiple mutexes support karta hai
    ✅ C++17 se available

    Kab use karo:
    → Jab bhi 2+ mutexes ek saath lock karne ho
    → Modern C++ code mein yahi preferred hai

    -------------------------------------------------------
    FIX 4 — try_lock WITH RETRY
    -------------------------------------------------------

    if (mtxA.try_lock()) {
        if (mtxB.try_lock()) {
            // Kaam karo
        } else {
            mtxA.unlock(); // Dusra nahi mila toh pehla bhi chhhodo!
        }
    }

    try_lock() non-blocking hai:
    - Lock mila → true, kaam karo
    - Lock nahi mila → false, ruko mat — aage badho

    IMPORTANT — Agar mtxB nahi mila:
    mtxA ZAROOR unlock karo!
    Warna partial hold rehega → deadlock/livelock ka risk.

    Retry ke beech delay kyun:
    - Bina delay ke dono threads ek saath retry karte rahenge.
    - Ek baar phir dono ek doosre ka wait karein — LIVELOCK!
    - Alag delay (id * 30ms) se ek thread pehle retry karti hai.

    -------------------------------------------------------
    DEADLOCK vs LIVELOCK vs STARVATION
    -------------------------------------------------------

    DEADLOCK:
    - Threads completely RUKI hain — koi progress nahi.
    - CPU usage: 0% (sab wait mein hain).
    - Detect karna easy — program hang ho jaata hai.

    LIVELOCK:
    - Threads ACTIVE hain par progress zero.
    - Dono ek doosre ke liye lock chhod dete hain.
    - CPU usage: High (busy loop mein hain).
    - Detect karna MUSHKIL — program hang nahi lagta.
    - Fix: Random/id-based delay.

    STARVATION:
    - Ek thread hamesha wait karti rehti hai.
    - Doosri threads baar baar lock le leti hain.
    - Thread "bhookhi" rehti hai — use resources nahi milte.
    - Fix: Fair mutex, priority scheduling.

    -------------------------------------------------------
    COMPARISON TABLE — Deadlock Fixes
    -------------------------------------------------------

    Fix              | Ease    | Deadlock | C++  | Recommended
                     |         | Proof    | Ver  |
    -----------------|---------|----------|------|------------
    Same Order       | Easy ✅ | YES ✅   | 11   | Small projects
    std::lock()      | Medium  | YES ✅   | 11   | Complex cases
    scoped_lock      | Easy ✅ | YES ✅   | 17   | ✅ BEST CHOICE
    try_lock + retry | Complex | Mostly   | 11   | Special cases

    -------------------------------------------------------
    GOLDEN RULES — Deadlock Se Bachne Ke Liye
    -------------------------------------------------------

    Rule 1 — Same Order:
    Team mein decide karo: "Hamesha A pehle, B baad mein"
    Kabhi opposite order mat karo.

    Rule 2 — scoped_lock use karo:
    2+ mutex chahiye toh scoped_lock(mtxA, mtxB) — ek line, safe.

    Rule 3 — Lock hold time kam rakho:
    Lock ke andar sirf zaruri kaam karo.
    Sleep, I/O, network calls — lock ke BAHAR karo.

    Rule 4 — try_lock mein partial hold mat rakho:
    Ek mila, doosra nahi → pehla bhi chhhod do aur retry karo.

    Rule 5 — Deadlock detect karo:
    Program hang ho gaya? gdb ya thread sanitizer use karo.
    g++ -fsanitize=thread file.cpp
*/