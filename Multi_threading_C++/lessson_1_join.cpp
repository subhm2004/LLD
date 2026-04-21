#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

// ============================================================
// VERSION 1 — Original (Basic Thread)
// ============================================================
void print(string msg, int count)
{
    for (int i = 1; i <= count; i++)
    {
        cout << msg << " " << i << endl;
    }
}

// ============================================================
// VERSION 2 — Thread with Sleep (Delay simulation)
// ============================================================
void printWithDelay(string msg, int count, int delayMs)
{
    for (int i = 1; i <= count; i++)
    {
        this_thread::sleep_for(chrono::milliseconds(delayMs));
        cout << "[Delay] " << msg << " " << i << endl;
    }
}

// ============================================================
// VERSION 3 — Thread ID (Kaun sa thread chal raha hai?)
// ============================================================
void printWithThreadId(string msg, int count)
{
    for (int i = 1; i <= count; i++)
    {
        cout << "[ThreadID: " << this_thread::get_id() << "] "
             << msg << " " << i << endl;
    }
}

// ============================================================
// VERSION 4 — Detach (Thread apne aap chale, main wait na kare)
// ============================================================
void backgroundTask(string msg, int count)
{
    for (int i = 1; i <= count; i++)
    {
        this_thread::sleep_for(chrono::milliseconds(200));
        cout << "[Detached] " << msg << " " << i << endl;
    }
}

// ============================================================
// VERSION 5 — Lambda Thread (Inline function)
// ============================================================
// Koi alag function nahi — seedha main mein likhenge

// ============================================================
// VERSION 6 — Multiple Threads (Vector of threads)
// ============================================================
void worker(int id)
{
    cout << "[Worker] Thread " << id << " shuru hua" << endl;
    this_thread::sleep_for(chrono::milliseconds(id * 100));
    cout << "[Worker] Thread " << id << " khatam hua" << endl;
}

// ============================================================
// VERSION 7 — joinable() check
// ============================================================
void simpleTask(string msg)
{
    cout << "[joinable] " << msg << endl;
}

// ============================================================
// VERSION 8 — hardware_concurrency
// ============================================================
// Seedha main mein use karenge

int main()
{
    // ----------------------------------------------------------
    // 1. BASIC THREAD — Original code
    // ----------------------------------------------------------
    cout << "====== 1. Basic Thread ======" << endl;
    {
        thread t1(print, "Hello", 5);
        thread t2(print, "Bye", 5);
        t1.join(); // Main thread t1 ke khatam hone ka wait karega
        t2.join(); // Main thread t2 ke khatam hone ka wait karega
    }

    // ----------------------------------------------------------
    // 2. THREAD WITH SLEEP — Delay ke saath
    // ----------------------------------------------------------
    cout << "\n====== 2. Thread with Sleep/Delay ======" << endl;
    {
        thread t1(printWithDelay, "Hello", 5, 300); // 300ms delay
        thread t2(printWithDelay, "Bye", 5, 500);   // 500ms delay
        t1.join();
        t2.join();
    }

    // ----------------------------------------------------------
    // 3. THREAD ID — Unique ID kaun sa thread hai
    // ----------------------------------------------------------
    cout << "\n====== 3. Thread ID ======" << endl;
    {
        thread t1(printWithThreadId, "Hello", 3);
        thread t2(printWithThreadId, "Bye", 3);
        t1.join();
        t2.join();
        // Main thread ka ID bhi dekh sakte ho
        cout << "[Main ThreadID]: " << this_thread::get_id() << endl;
    }

    // ----------------------------------------------------------
    // 4. DETACH — Thread ko free chhod do
    // ----------------------------------------------------------
    cout << "\n====== 4. Detach ======" << endl;
    {
        thread t1(backgroundTask, "Background", 3);
        t1.detach(); // Ab t1 apne aap chalega, join nahi karenge
        // ⚠️ Detach ke baad t1 ko join() nahi kar sakte
        // Main thread aage badh jaata hai
        cout << "[Main] Detached thread ko chhod diya, main aage badh gaya" << endl;
        this_thread::sleep_for(chrono::milliseconds(1000)); // Thoda wait karo output dekhne ke liye
    }

    // ----------------------------------------------------------
    // 5. LAMBDA THREAD — Alag function banane ki zarurat nahi
    // ----------------------------------------------------------
    cout << "\n====== 5. Lambda Thread ======" << endl;
    {
        thread t1([](string msg, int count)
                  {
            for (int i = 1; i <= count; i++)
                cout << "[Lambda] " << msg << " " << i << endl; }, "Hello", 5);

        thread t2([](string msg, int count)
                  {
            for (int i = 1; i <= count; i++)
                cout << "[Lambda] " << msg << " " << i << endl; }, "Bye", 5);

        t1.join();
        t2.join();
    }

    // ----------------------------------------------------------
    // 6. MULTIPLE THREADS — Vector mein rakho saari threads
    // ----------------------------------------------------------
    cout << "\n====== 6. Multiple Threads (Vector) ======" << endl;
    {
        vector<thread> threads;

        for (int i = 1; i <= 5; i++)
            threads.push_back(thread(worker, i)); // 5 threads ek saath

        for (auto &t : threads)
            t.join(); // Sabko join karo
    }

    // ----------------------------------------------------------
    // 7. JOINABLE CHECK — Thread join karne layak hai ya nahi
    // ----------------------------------------------------------
    cout << "\n====== 7. joinable() Check ======" << endl;
    {
        thread t1(simpleTask, "Main kaam kar raha hoon");

        cout << "join se pehle joinable: "
             << (t1.joinable() ? "YES" : "NO") << endl;

        t1.join();

        cout << "join ke baad joinable: "
             << (t1.joinable() ? "YES" : "NO") << endl;
        // ✅ join() ke baad thread "dead" ho jaata hai
        // joinable() false ho jaata hai
    }

    // ----------------------------------------------------------
    // 8. HARDWARE CONCURRENCY — System ke actual cores
    // ----------------------------------------------------------
    cout << "\n====== 8. Hardware Concurrency ======" << endl;
    {
        unsigned int cores = thread::hardware_concurrency();
        cout << "Tera PC ek saath " << cores << " threads chala sakta hai!" << endl;
        cout << "Matlab " << cores << " threads truly parallel chalenge" << endl;
        cout << "Isse zyada threads banaoge toh OS context-switch karega" << endl;
    }

    return 0;
}

/*
    THREADING CONCEPTS — Complete Hinglish Explanation
    ===================================================

    1. BASIC THREAD:
       - thread t(functionName, arg1, arg2, ...);
       - Thread create hote hi parallel chalna shuru ho jaata hai.
       - t.join() → Main thread wait karega is thread ke khatam hone tak.

    2. SLEEP / DELAY:
       - this_thread::sleep_for(chrono::milliseconds(500));
       - Current thread ko 500ms ke liye sula do.
       - Parallel execution simulate karne ke liye use hota hai.
       - sleep() (unistd.h) — seconds mein, Linux only.
       - sleep_for() — milliseconds/microseconds mein, cross-platform ✅

    3. THREAD ID:
       - this_thread::get_id() → Current thread ka unique ID deta hai.
       - Har thread ka ID alag hota hai — debug karne mein helpful.
       - Main thread ka bhi apna ID hota hai.

    4. JOIN vs DETACH:
       - join()   → Main thread rukta hai — thread khatam ho tab aage badhta hai.
       - detach() → Thread free ho jaata hai — main rukta nahi.
       - ⚠️ Detach ke baad join() NAHI kar sakte — crash hoga!
       - ⚠️ join() ya detach() ZAROOR karo — varna program crash!

    5. LAMBDA THREAD:
       - Alag function banane ki zarurat nahi.
       - thread t([](args){ body }, arg1, arg2);
       - Chhote tasks ke liye clean aur readable.

    6. VECTOR OF THREADS:
       - Jab bahut saari threads ek saath chahiye.
       - vector<thread> threads;
       - threads.push_back(thread(func, args));
       - Baad mein loop mein sab ko join karo.

    7. JOINABLE():
       - t.joinable() → true  : Thread abhi chal rahi hai, join ho sakti hai.
       - t.joinable() → false : Thread khatam ho gayi ya detach ho gayi.
       - Join karne se pehle check karo — safe coding practice.

    8. HARDWARE CONCURRENCY:
       - thread::hardware_concurrency() → Actual CPU cores/threads count.
       - Isse zyada threads = Context switching = Slow ho sakta hai.
       - Optimal thread count = hardware_concurrency() ke barabar.

    QUICK REFERENCE:
    -------------------------------------------------------
    Concept              | Code
    -------------------------------------------------------
    Thread banana        | thread t(func, args...);
    Wait karna           | t.join();
    Free chhodna         | t.detach();
    Check karna          | t.joinable();
    ID dekhna            | this_thread::get_id();
    Sulana               | this_thread::sleep_for(...);
    Cores count          | thread::hardware_concurrency();
    Lambda thread        | thread t([](){ ... });
    Multiple threads     | vector<thread> v;
    -------------------------------------------------------
*/