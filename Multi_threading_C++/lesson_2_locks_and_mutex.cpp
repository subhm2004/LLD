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