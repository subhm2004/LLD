// Agar aapka matlab hai: "Order fixed ho (Pehle A, phir B, phir C)"
/*
Agar aap chahte ho ki hamesha pehle A print ho, phir B, phir C... toh asliyat ye hai ki aapko multi-threading ki zaroorat hi nahi hai. Kyunki agar threads ek doosre ka wait karke line se chalenge, toh wo "Parallel" nahi "Sequential" ho gaye.

Threads ko ek fixed sequence mein chalana chahte ho, toh uske liye hum Condition Variables use karte hain.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable> // Nayi header file sequence ke liye

using namespace std;

mutex mtx;
condition_variable cv;
int turn = 0; // 0 for TaskA, 1 for TaskB, 2 for TaskC

void taskA()
{
    for (int i = 0; i < 5; i++)
    {
        // NOTE : jab hum Condition Variables (baari-baari kaam karne wala logic) use karte hain, toh humein unique_lock hi use karna padta hai.
        unique_lock<mutex> lock(mtx);
        // Wait karo jab tak turn 0 na ho
        cv.wait(lock, []
                { return turn == 0; });

        cout << "Task A: " << i << endl;

        turn = 1;        // Agli baari TaskB ki
        cv.notify_all(); // Sabko jagao ki turn change hui hai
    }
}

void taskB()
{
    for (int i = 0; i < 5; i++)
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, []
                { return turn == 1; });

        cout << "Task B: " << i << endl;

        turn = 2; // Agli baari TaskC ki
        cv.notify_all();
    }
}

void taskC()
{
    for (int i = 0; i < 5; i++)
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, []
                { return turn == 2; });

        cout << "Task C: " << i << endl;

        turn = 0; // Wapas TaskA ki baari
        cv.notify_all();
    }
}

int main()
{
    thread t1(taskA);
    thread t2(taskB);
    thread t3(taskC);

    t1.join();
    t2.join();
    t3.join();
    return 0;
}

/*
    CONDITION VARIABLES — Detailed Hinglish Explanation
    ====================================================

    YEH CODE KYA KARTA HAI:
    - 3 threads hain: taskA, taskB, taskC
    - Hamesha fixed order mein chalte hain: A → B → C → A → B → C...
    - Yeh cheez "turn" variable aur condition_variable se achieve hoti hai

    -------------------------------------------------------
    FLOW DIAGRAM (5 iterations):
    -------------------------------------------------------

    Iteration 1:
    turn=0 → TaskA chala → turn=1 → notify_all()
    turn=1 → TaskB chala → turn=2 → notify_all()
    turn=2 → TaskC chala → turn=0 → notify_all()

    Iteration 2:
    turn=0 → TaskA chala → turn=1 → notify_all()
    ... aur yahi cycle chalti rehti hai 5 baar tak

    -------------------------------------------------------
    TEEN MAIN COMPONENTS:
    -------------------------------------------------------

    1. mutex mtx:
       - Ek taala jo ensure karta hai ki ek waqt mein
         sirf ek thread "turn" variable ko read/write kare.
       - Bina mutex ke do threads ek saath turn change
         kar sakti thi → Chaos!

    2. condition_variable cv:
       - Ek "signal system" hai threads ke beech.
       - Thread ko bolta hai: "Ruk jao jab tak tumhari baari na aaye"
       - notify_all() se saari sleeping threads ek baar jaagti hain
         aur check karti hain ki unki baari aayi ya nahi.

    3. int turn = 0:
       - Global "token" hai jo decide karta hai kaun chalega.
       - turn == 0 → TaskA ki baari
       - turn == 1 → TaskB ki baari
       - turn == 2 → TaskC ki baari

    -------------------------------------------------------
    cv.wait() KA KAAM — Step by Step:
    -------------------------------------------------------

    cv.wait(lock, []{ return turn == 0; });

    Step 1: Predicate check karo (turn == 0 hai?)
            ↓ YES → Seedha aage badho, wait mat karo
            ↓ NO  → Step 2 pe jao

    Step 2: mutex ka lock AUTOMATICALLY chhod do
            (Taaki doosri threads kaam kar sakein)

    Step 3: Thread "sleep" mode mein chali jaati hai
            (CPU waste nahi hota — thread park ho jaati hai)

    Step 4: notify_all() aane pe jaago
            mutex dobara lo
            predicate dobara check karo → Step 1 pe wapas

    -------------------------------------------------------
    notify_one() vs notify_all():
    -------------------------------------------------------

    notify_one():
    - Sirf ek sleeping thread ko jagata hai.
    - Kaunsi? → OS decide karta hai (random).
    - Fast hai kyunki kam kaam karta hai.
    - ⚠️ Risk: Galat thread jaag sakti hai jo wait karte karte
               wapas so jaaye → doosri thread kabhi na jaage!

    notify_all():
    - Saari sleeping threads ko ek saath jagata hai.
    - Sab apna predicate check karti hain.
    - Jis ki baari hai woh kaam karta hai, baaki wapas so jaati hain.
    - Thoda slow — extra wakeups hote hain.
    - ✅ Is code mein ZAROOR notify_all() chahiye tha kyunki
       3 alag threads hain aur kisi bhi ek ko jaagna pad sakta hai.

    -------------------------------------------------------
    unique_lock KYUN — lock_guard KYUN NAHI:
    -------------------------------------------------------

    lock_guard:
    - Scope end pe unlock hota hai — beech mein nahi.
    - cv.wait() ko beech mein mutex unlock karna padta hai
      (Taaki doosri threads enter kar sakein).
    - lock_guard yeh kaam nahi kar sakta → COMPILE ERROR.

    unique_lock:
    - cv.wait() ke andar automatically unlock/relock hota hai.
    - Isliye condition_variable ke saath SIRF unique_lock kaam karta hai.
    - Yeh ek hard rule hai C++ mein.

    -------------------------------------------------------
    SPURIOUS WAKEUP — Chupa hua danger:
    -------------------------------------------------------

    - Kuch baar thread bina notify ke khud jaag jaati hai.
    - Isse "Spurious Wakeup" kehte hain — OS ki wajah se hota hai.
    - Agar hum sirf cv.wait(lock) likhte (bina predicate ke):
      → Thread galat waqt pe jaag sakti thi → Wrong output!

    - Predicate (lambda) iska solution hai:
      cv.wait(lock, []{ return turn == 0; });
      → Jaago, check karo, agar turn sahi nahi toh wapas so jao.
      → Spurious wakeup automatically handle ho jaata hai.

    -------------------------------------------------------
    PARALLEL vs SEQUENTIAL — Important Concept:
    -------------------------------------------------------

    Yeh code technically "parallel" nahi hai:
    - Ek waqt mein sirf ek thread kaam karti hai.
    - Baaki dono wait mode mein hoti hain.
    - Toh yeh "synchronized sequential execution" hai.

    Sach mein parallel kab hota hai:
    - Jab threads independently kaam karein — koi fixed order nahi.
    - Jaise image processing: Thread 1 top half, Thread 2 bottom half.
    - Dono ek saath bina wait kiye chalein.

    Fixed order chahiye → Condition Variable (yeh code)
    Maximum speed chahiye → True parallel (no fixed order)

    -------------------------------------------------------
    COMPLETE FLOW VISUAL:
    -------------------------------------------------------

    Start:  turn = 0
            T1(A) jaag rahi | T2(B) so rahi | T3(C) so rahi

    Round 1:
    T1 → kaam kiya → turn=1 → notify_all()
    T2 → jaagi, turn==1 ✅ → kaam kiya → turn=2 → notify_all()
    T3 → jaagi, turn==2 ✅ → kaam kiya → turn=0 → notify_all()

    Round 2:
    T1 → jaagi, turn==0 ✅ → kaam kiya → turn=1 → notify_all()
    ... yahi cycle 5 baar

    End: teenon threads khatam → main return 0

    -------------------------------------------------------
    QUICK REFERENCE:
    -------------------------------------------------------
    cv.wait(lock, pred)  → Ruko jab tak pred true na ho
    cv.notify_one()      → Ek thread jagao
    cv.notify_all()      → Saari threads jagao
    turn variable        → Token — kaun chalega decide karta hai
    unique_lock          → cv ke saath ZAROOR use karo
    Predicate/Lambda     → Spurious wakeup se bachao
*/