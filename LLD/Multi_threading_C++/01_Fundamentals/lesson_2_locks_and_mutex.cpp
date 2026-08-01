#include <iostream>
#include <thread>
#include <unistd.h> // sleep() function ke liye
#include <mutex>    // Nayi header file

using namespace std;

mutex mtx; // Ye raha hamara taala

void taskA()
{
    for (int i = 0; i < 10; i++)
    {
        sleep(1);
        // sleep function ka kaam hai thread ko thodi der ke liye "rok dena" ya sula dena.
        // is case me ye thream ko 1 second tak sula dega taki hum dekh paye parallel execution kse ho rhi hai

        mtx.lock(); // Lock lagaya (Ab sirf ye thread likh payega)
        cout << "Task A: " << " " << i << endl;
        mtx.unlock(); // Lock khola
    }
}

void taskB()
{
    for (int i = 0; i < 10; i++)
    {
        sleep(1);
        mtx.lock(); // Lock lagaya
        cout << "Task B: " << " " << i << endl;
        mtx.unlock(); // Lock khola
    }
}
/*
Abhi humne mtx.lock() aur mtx.unlock() manually kiya hai. Ismein ek khatra hota hai: agar code mein lock ke baad koi error (exception) aa jaye, toh unlock() kabhi call hi nahi hoga aur program Deadlock mein phans jayega.

lock_guard ek "automatic" manager hai. Jaise hi function khatam hoga ya bracket {} khatam hoga, ye apne aap lock khol dega.
*/
void taskC(int id, string name)
{
    for (int i = 0; i < 10; i++)
    {
        sleep(1);
        {
            std::lock_guard<std::mutex> lock(mtx); // Bracket shuru hote hi lock
            cout << "Task C: " << i << " " << "Thread " << id << " (" << name << ") is running" << endl;
        } // Bracket khatam hote hi apne aap unlock!
    }
}
int main()
{
    // Threads create ho rahe hain aur parallelly chalna shuru kar denge
    thread t1(taskA);
    thread t2(taskB);
    thread t3(taskC, 1, "Shubham ji ");

    // Main thread wait karega t1 aur t2 ke khatam hone ka
    t1.join();
    t2.join();
    t3.join();

    cout << "Mera PC ek saath " << thread::hardware_concurrency() << " threads chala sakta hai!" << endl;
    return 0;
}

/*
Pro Tip: Hardware Concurrency
Kya aapko pata hai aapka computer ek saath kitne threads "asliyat" mein chala sakta hai? Aap ye check kar sakte ho:
cout << "Mera PC ek saath " << thread::hardware_concurrency() << " threads chala sakta hai!" << endl;
*/

/*
    THREADS + MUTEX — Detailed Hinglish Explanation
    =================================================

    -------------------------------------------------------
    YEH PROGRAM KYA KARTA HAI — Overview
    -------------------------------------------------------

    - 3 threads ek saath (parallel) start hoti hain: t1, t2, t3
    - Teeno ek loop chalate hain 10 baar
    - Har iteration mein 1 second ruk ke cout pe print karte hain
    - mutex ensure karta hai ki cout pe ek waqt sirf ek thread likhe

    -------------------------------------------------------
    PARALLEL EXECUTION KYA HOTA HAI
    -------------------------------------------------------

    Bina threads ke (sequential):
    TaskA 0 → TaskA 1 → ... → TaskA 9
    TaskB 0 → TaskB 1 → ... → TaskB 9
    TaskC 0 → TaskC 1 → ... → TaskC 9
    Total time: 30 seconds (10+10+10)

    Threads ke saath (parallel):
    TaskA, TaskB, TaskC — teeno ek saath chalte hain
    Total time: ~10 seconds (sab ek saath)

    Yahi fayda hai multithreading ka — time bachta hai!

    -------------------------------------------------------
    sleep(1) KYU LAGAYA — Step by Step
    -------------------------------------------------------

    sleep(1) → Thread ko 1 second ke liye "sula do"

    Bina sleep ke:
    - Loop itni tezi se chalta hai ki output ek hi thread
      ka baar baar aata hai — parallel execution dikh hi nahi paata.

    Sleep ke saath:
    - Har thread 1 second rukti hai.
    - Is dauraan doosri threads CPU use karti hain.
    - Output mein A, B, C mix hokar aate hain — parallel clearly dikhta hai.

    sleep() — unistd.h se aata hai, seconds mein kaam karta hai, Linux/Mac pe.
    sleep_for() — chrono se aata hai, milliseconds mein bhi kaam karta hai,
                  cross-platform (Windows pe bhi).

    -------------------------------------------------------
    RACE CONDITION — Mutex Kyun Zaruri Hai
    -------------------------------------------------------

    cout ek shared resource hai — sab threads iska use karti hain.

    Bina mutex ke kya hoga:
    Thread A likhna shuru karta hai: "Task A: 3"
    Beech mein Thread B ghus jaata hai: "Task B: 7"
    Output: "Task A: Task B: 3 7" ← Garbled/Mixed output!

    Yeh Race Condition hai — do threads ek saath
    ek resource use kar rahi hain bina permission ke.

    Mutex is problem ka solution hai.

    -------------------------------------------------------
    MUTEX KYA HAI — Taale ki Misaal
    -------------------------------------------------------

    mutex mtx; ← Ek global taala banaya

    Ghar ka taala sochlo:
    - Sirf ek hi insaan andar aa sakta hai ek waqt mein.
    - Baaki log bahar wait karte hain.
    - Kaam khatam → taala kholo → agla andar aaye.

    Mutex bilkul aisa hi kaam karta hai threads ke liye:
    - mtx.lock()   → Taala lagao (sirf main likhonga)
    - cout << ...  → Kaam karo (safely print karo)
    - mtx.unlock() → Taala kholo (ab doosra aa sakta hai)

    -------------------------------------------------------
    RAW MUTEX — TaskA aur TaskB (mtx.lock / mtx.unlock)
    -------------------------------------------------------

    mtx.lock();
    cout << "Task A: " << i << endl;
    mtx.unlock();

    Yeh seedha tarika hai — simple aur direct.

    PROBLEM — Exception ka Khatra:
    mtx.lock();
    // Yahan agar exception aa jaaye (crash, error, etc.)
    // toh unlock() kabhi call nahi hoga!
    // Mutex hamesha ke liye locked reh jaayega.
    // Doosri threads kabhi lock nahi le paayengi.
    // Program hang ho jaayega — yahi DEADLOCK hai!
    mtx.unlock(); // Yeh line execute hi nahi hogi

    Isliye production code mein raw mutex avoid karo.

    -------------------------------------------------------
    DEADLOCK KYA HOTA HAI
    -------------------------------------------------------

    Situation 1 — Exception wala deadlock (upar wala):
    Thread lock leke crash ho gayi → unlock kabhi nahi hua
    → Baaki threads hamesha wait karti rahein → Program hang

    Situation 2 — Classic deadlock (2 threads, 2 mutex):
    Thread1: mtxA lock kiya, mtxB ka wait kar raha hai
    Thread2: mtxB lock kiya, mtxA ka wait kar raha hai
    → Dono ek doosre ka wait karte rahenge forever → DEADLOCK

    Solution: lock_guard ya unique_lock use karo (RAII)

    -------------------------------------------------------
    LOCK_GUARD — TaskC (Automatic Safe Lock)
    -------------------------------------------------------

    {
        lock_guard<mutex> lock(mtx); ← { mein ghuste hi auto-lock
        cout << "Task C: " << i << endl;
    } ← } se nikalte hi auto-unlock — guaranteed!

    lock_guard RAII pattern follow karta hai:
    RAII = Resource Acquisition Is Initialization
    Matlab: Object bana = resource lo, Object destroy = resource do

    lock_guard ke fayde:
    ✅ Exception aaye tab bhi unlock guaranteed
    ✅ Unlock bhoolne ka koi chance nahi
    ✅ Scope clearly dikhta hai — exactly pata hai kab unlock hoga
    ✅ Raw mutex se zyada safe

    lock_guard ki limit:
    ❌ Beech mein manually unlock NAHI kar sakte
    ❌ Condition variables ke saath kaam nahi karta
    ❌ Re-lock nahi kar sakte

    Agar beech mein unlock chahiye → unique_lock use karo

    -------------------------------------------------------
    { } CURLY BRACKETS KA ROLE — TaskC mein
    -------------------------------------------------------

    void taskC() {
        for (...) {
            sleep(1);
            {                              ← Inner scope shuru
                lock_guard<mutex> lock(mtx); ← Lock liya
                cout << ... << endl;
            }                              ← Inner scope khatam → auto-unlock
            // Yahan lock nahi hai — doosra kaam kar sakte ho
        }
    }

    Agar brackets nahi lagate:
    void taskC() {
        for (...) {
            lock_guard<mutex> lock(mtx);
            cout << ... << endl;
            sleep(1); // Sleep ke dauran bhi lock laga rahega!
            // Matlab 1 second tak koi aur thread print nahi kar payega
        }
    }

    Brackets se lock sirf cout ke waqt lagta hai —
    sleep ke dauran doosri threads freely kaam kar sakti hain.

    -------------------------------------------------------
    THREAD CREATION — main() mein kya hua
    -------------------------------------------------------

    thread t1(taskA);
    - taskA function ko ek nayi thread mein start kar diya.
    - t1 create hote hi taskA parallel chalne laga.

    thread t2(taskB);
    - taskB bhi parallel start — t1 aur t2 dono ek saath chal rahe.

    thread t3(taskC, 1, "Shubham ji");
    - taskC ko arguments ke saath thread mein pass kiya.
    - 1 → id parameter, "Shubham ji" → name parameter.
    - thread(function, arg1, arg2, ...) → yahi syntax hai.

    -------------------------------------------------------
    JOIN — Rukne ka Signal
    -------------------------------------------------------

    t1.join();
    t2.join();
    t3.join();

    join() ka matlab:
    - "Main thread yahan ruk jaao jab tak t1 khatam na ho jaye"
    - Phir t2 ka wait, phir t3 ka wait.

    Agar join() na karein:
    - main() return ho jaayega.
    - Program khatam — threads beech mein hi chhoot jaayengi.
    - Crash ya undefined behavior ho sakta hai.

    join() vs detach():
    join()   → Main rukta hai, thread ka wait karta hai ✅
    detach() → Main nahi rukta, thread apne aap chalti hai
               (background tasks ke liye — logs, cleanup, etc.)

    -------------------------------------------------------
    HARDWARE CONCURRENCY
    -------------------------------------------------------

    thread::hardware_concurrency()
    → Tera CPU ek waqt mein kitne threads SACH MEIN parallel chala sakta hai.

    Example output: 8
    Matlab: 4 cores × 2 hyperthreads = 8 parallel threads

    Isse zyada threads banao:
    - OS "context switching" karta hai.
    - Threads baar baar CPU pe aate-jaate rehte hain.
    - Overhead badhta hai — speed kam ho sakti hai.

    Best practice:
    - Thread count = hardware_concurrency() ke barabar rakho.
    - Zyada threads = Diminishing returns (fayda kam, kharcha zyada).

    -------------------------------------------------------
    COMPLETE FLOW — Sab Mila Ke
    -------------------------------------------------------

    main() shuru hua
        ↓
    t1(taskA), t2(taskB), t3(taskC) — teeno parallel start
        ↓
    Har thread apna loop chalati hai (10 iterations)
        ↓
    Har iteration:
        sleep(1) → 1 second ruko
        mutex lock → cout → mutex unlock
        ↓
    t1.join() → main rukta hai → t1 khatam
    t2.join() → main rukta hai → t2 khatam
    t3.join() → main rukta hai → t3 khatam
        ↓
    hardware_concurrency print
        ↓
    return 0 — Program khatam

    -------------------------------------------------------
    COMPARISON TABLE
    -------------------------------------------------------

    Feature          | Raw Mutex      | lock_guard
    -----------------|----------------|------------------
    Lock kaise       | mtx.lock()     | Auto on { enter
    Unlock kaise     | mtx.unlock()   | Auto on } exit
    Exception safe   | NO ❌          | YES ✅
    Manual unlock    | YES            | NO
    Deadlock risk    | HIGH ⚠️        | LOW ✅
    Code simplicity  | Simple         | Cleaner
    Use karo jab     | Avoid karo     | Default choice

    -------------------------------------------------------
    GOLDEN RULES
    -------------------------------------------------------

    ✅ Simple lock chahiye          → lock_guard
    ✅ Beech mein unlock chahiye    → unique_lock
    ✅ Condition variable chahiye   → unique_lock (mandatory)
    ✅ sleep() sirf cout se bahar   → lock hold mat karo sleep mein
    ✅ Har thread ko join() karo    → Crash se bachao
    ❌ Raw mutex production mein    → Avoid karo
    ❌ sleep() lock ke andar        → Doosri threads bhookhi raheingi
*/