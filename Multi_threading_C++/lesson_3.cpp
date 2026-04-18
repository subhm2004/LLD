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