#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <atomic>
using namespace std;

/*
    THREAD POOL KYA HOTA HAI?
    - Ek fixed number of threads pehle se bana ke rakhte hain.
    - Kaam aata hai toh un threads ko dete hain.
    - Thread khatam hone ke baad woh thread pool mein wapas aa jaata hai.
    - Baar baar naye threads banana nahi padta — fast & efficient!
*/

// ============================================================
// THREAD POOL CLASS
// ============================================================
class ThreadPool
{
private:
    vector<thread> workers;        // Pool mein rakhe hue threads
    queue<function<void()>> tasks; // Pending kaam ki queue
    mutex queueMtx;                // Queue ko protect karne ke liye
    condition_variable cv;         // Thread ko jagane ke liye
    atomic<bool> stopPool;         // Pool band karne ka signal

public:
    // ----------------------------------------------------------
    // CONSTRUCTOR — Pool banana aur threads start karna
    // ----------------------------------------------------------
    ThreadPool(int numThreads) : stopPool(false)
    {
        cout << "[Pool] " << numThreads << " threads bana rahe hain..." << endl;

        for (int i = 0; i < numThreads; i++)
        {
            workers.push_back(thread([this, i]()
                                     {
                cout << "[Worker " << i << "] Ready & waiting..." << endl;

                while (true)
                {
                    function<void()> task;  // Jo kaam milega woh yahan store hoga

                    {
                        unique_lock<mutex> lock(queueMtx);

                        // Kaam aane tak ya pool band hone tak wait karo
                        cv.wait(lock, [this]()
                        {
                            return !tasks.empty() || stopPool;
                        });

                        // Pool band ho gaya aur koi kaam nahi → exit
                        if (stopPool && tasks.empty())
                        {
                            cout << "[Worker " << i << "] Kaam khatam, exit kar raha hoon." << endl;
                            return;
                        }

                        // Queue se ek kaam uthao
                        task = tasks.front();
                        tasks.pop();
                        cout << "[Worker " << i << "] Kaam uthaya queue se." << endl;
                    }

                    task(); // Lock ke bahar kaam karo — doosre threads bhi kaam karein
                } }));
        }
    }

    // ----------------------------------------------------------
    // enqueue — Naya kaam pool ko do
    // ----------------------------------------------------------
    void enqueue(function<void()> task)
    {
        {
            lock_guard<mutex> lock(queueMtx);
            tasks.push(task); // Queue mein daalo
            cout << "[Pool] Naya kaam queue mein dala. Queue size: " << tasks.size() << endl;
        }
        cv.notify_one(); // Ek sleeping thread ko jagao
    }

    // ----------------------------------------------------------
    // DESTRUCTOR — Pool band karna
    // ----------------------------------------------------------
    ~ThreadPool()
    {
        {
            lock_guard<mutex> lock(queueMtx);
            stopPool = true; // Sabko signal do — band hone wala hai
        }

        cv.notify_all(); // Saari sleeping threads ko jagao

        for (auto &worker : workers)
        {
            if (worker.joinable())
                worker.join(); // Sabka wait karo
        }

        cout << "[Pool] Sab threads band ho gaye. Pool destroy!" << endl;
    }
};

// ============================================================
// MAIN — Thread Pool Use Karna
// ============================================================
int main()
{
    cout << "====== THREAD POOL DEMO ======\n"
         << endl;

    // 3 threads ka pool banaya
    ThreadPool pool(3);

    cout << "\n--- 10 kaam queue mein de rahe hain ---\n"
         << endl;

    // 10 tasks pool ko do
    for (int i = 1; i <= 10; i++)
    {
        pool.enqueue([i]()
                     {
            cout << "[Task " << i << "] Shuru hua | Thread: "
                 << this_thread::get_id() << endl;

            this_thread::sleep_for(chrono::milliseconds(i * 100)); // Kaam simulate

            cout << "[Task " << i << "] Khatam hua!" << endl; });
    }

    cout << "\n--- Saare tasks queue mein hain, threads kaam kar rahe hain ---\n"
         << endl;

    this_thread::sleep_for(chrono::seconds(4)); // Kaam hone do

    cout << "\n--- Pool band ho raha hai ---\n"
         << endl;

    // Destructor call hoga — pool gracefully band hoga
    return 0;
}

/*
    THREAD POOL — Complete Hinglish Explanation
    ============================================

    -------------------------------------------------------
    THREAD POOL KYU CHAHIYE — Problem Without It
    -------------------------------------------------------

    Bina Thread Pool ke:
    for (int i = 0; i < 1000; i++) {
        thread t(doWork, i);  // Har kaam ke liye naya thread!
        t.detach();
    }

    PROBLEMS:
    1. Thread creation expensive hai — OS ke saath baat karni padti hai
    2. 1000 threads ek saath → RAM waste, context switching hell
    3. Koi control nahi — threads unlimited ban sakte hain
    4. Thread destroy karna bhi costly hai

    Thread Pool ke saath:
    ThreadPool pool(4);           // Sirf 4 threads — hamesha ke liye
    for (int i = 0; i < 1000; i++)
        pool.enqueue(doWork);     // Kaam queue mein — threads reuse hote hain

    BENEFITS:
    ✅ Thread creation cost sirf ek baar — startup pe
    ✅ Fixed threads — memory predictable
    ✅ Kaam queue mein wait karta hai — koi thread waste nahi
    ✅ Fast — threads pehle se ready hain

    -------------------------------------------------------
    THREAD POOL KE 4 MAIN PARTS
    -------------------------------------------------------

    1. Workers (vector<thread>):
       - Pehle se bane hue threads.
       - Hamesha ek loop mein chalte hain.
       - Kaam milne ka wait karte hain.
       - Kaam karo → wapas wait mein.

    2. Task Queue (queue<function<void()>>):
       - Pending kaamon ki line.
       - Jo pehle aaya woh pehle jayega (FIFO).
       - function<void()> → koi bhi kaam store kar sakte hain.

    3. Mutex + Condition Variable:
       - Queue ko protect karta hai — ek waqt ek thread access kare.
       - cv.wait() → thread soo jaata hai jab koi kaam nahi.
       - cv.notify_one() → naya kaam aaya → ek thread jagao.

    4. stopPool (atomic<bool>):
       - Pool band karne ka signal.
       - true hone pe threads apna loop exit karte hain.
       - atomic hai — safely sab threads padh sakti hain.

    -------------------------------------------------------
    WORKER THREAD KA LOOP — Step by Step
    -------------------------------------------------------

    while (true) {
        // 1. Lock lo
        unique_lock<mutex> lock(queueMtx);

        // 2. Wait karo jab tak:
        //    - Queue mein kaam ho, YA
        //    - stopPool true ho
        cv.wait(lock, [this]{ return !tasks.empty() || stopPool; });

        // 3. Band hone ka signal aur koi kaam nahi → exit
        if (stopPool && tasks.empty()) return;

        // 4. Queue se kaam uthao
        task = tasks.front();
        tasks.pop();

        // 5. Lock chhhodo — kaam karo
    }
    task(); // Lock ke BAHAR — parallel execution possible

    WHY task() LOCK KE BAHAR:
    - Agar lock ke andar kaam karein toh ek waqt sirf ek thread kaam kare
    - Baaki threads queue access nahi kar paayengi
    - Lock ke bahar = sab threads parallel kaam kar sakti hain ✅

    -------------------------------------------------------
    ENQUEUE — Kaam Dena
    -------------------------------------------------------

    void enqueue(function<void()> task) {
        lock_guard<mutex> lock(queueMtx);  // Queue lock karo
        tasks.push(task);                  // Kaam daalo
        cv.notify_one();                   // Ek thread jagao
    }

    notify_one() kyun notify_all() nahi:
    - Sirf ek kaam aaya → sirf ek thread chahiye
    - notify_all() se sab threads jaagti → sirf ek kaam uthati
    - Baaki sab wapas so jaati → Wasteful!
    - notify_one() = efficient ✅

    -------------------------------------------------------
    DESTRUCTOR — Graceful Shutdown
    -------------------------------------------------------

    ~ThreadPool() {
        stopPool = true;     // Signal do
        cv.notify_all();     // Saari threads jagao
        for (auto& w : workers)
            w.join();        // Sabka wait karo
    }

    Graceful shutdown kyun zaruri:
    - Abruptly band kiya → Pending tasks incomplete reh jaayenge
    - join() ensure karta hai sab threads apna current kaam finish karein
    - Phir exit karein → Clean shutdown ✅

    -------------------------------------------------------
    REAL WORLD MEIN THREAD POOL KAHAN USE HOTA HAI
    -------------------------------------------------------

    1. Web Servers (Apache, Nginx):
       - Har request ke liye naya thread nahi
       - Thread pool hai — request aai → ek thread assign

    2. Database Connection Pool:
       - DB connections expensive hain
       - Pool mein ready connections — request aai → assign

    3. Game Engines:
       - Physics, rendering, AI — alag alag thread pools
       - Parallel processing for performance

    4. Android/iOS Apps:
       - Background tasks ke liye thread pool
       - UI thread ko block mat karo

    -------------------------------------------------------
    OPTIMAL THREAD COUNT KYA HONA CHAHIYE
    -------------------------------------------------------

    CPU-bound tasks (calculations, sorting):
    threads = hardware_concurrency()
    Jitne cores utne threads — zyada se context switching

    I/O-bound tasks (network, file, DB):
    threads = hardware_concurrency() * 2  (ya zyada)
    I/O wait mein thread kuch nahi karta — zyada threads useful

    Formula:
    threads = cores * (1 + wait_time / compute_time)

    -------------------------------------------------------
    COMPARISON TABLE
    -------------------------------------------------------

    Feature          | Bina Pool      | Thread Pool
    -----------------|----------------|------------------
    Thread creation  | Har kaam pe    | Sirf ek baar
    Memory           | Unpredictable  | Fixed & controlled
    Speed            | Slow (create)  | Fast (reuse)
    Max threads      | Unlimited ⚠️   | Fixed ✅
    Complex code     | Simple         | Thoda complex
    Production use   | Never ❌       | Always ✅

    -------------------------------------------------------
    GOLDEN RULES
    -------------------------------------------------------

    ✅ Production mein hamesha Thread Pool use karo
    ✅ Thread count = hardware_concurrency() se shuru karo
    ✅ task() hamesha lock ke BAHAR execute karo
    ✅ Destructor mein join() zaroor karo — graceful shutdown
    ✅ stopPool atomic<bool> rakho — safely sab threads padh sakein
    ❌ Har kaam ke liye naya thread mat banao
    ❌ Lock ke andar time-consuming kaam mat karo
*/