#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
using namespace std;

queue<int> buffer;
const int CAPACITY = 5;
mutex mtx;
condition_variable cv_prod, cv_cons;
bool done = false;

void producer(int id)
{
    for (int i = 0; i < 10; ++i)
    {
        unique_lock<mutex> lock(mtx);
        cv_prod.wait(lock, []
                     { return buffer.size() < CAPACITY; });
        int item = id * 100 + i;
        buffer.push(item);
        cout << "Producer " << id << " produced: " << item
             << " [Buffer size: " << buffer.size() << "]" << endl;
        lock.unlock();
        cv_cons.notify_one();
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void consumer(int id)
{
    while (true)
    {
        unique_lock<mutex> lock(mtx);
        cv_cons.wait(lock, []
                     { return !buffer.empty() || done; });

        if (buffer.empty() && done)
            break;

        int item = buffer.front();
        buffer.pop();
        cout << "Consumer " << id << " consumed: " << item
             << " [Buffer size: " << buffer.size() << "]" << endl;
        lock.unlock();
        cv_prod.notify_one();
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

int main()
{
    thread p1(producer, 1);
    thread c1(consumer, 1);

    p1.join();
    {
        lock_guard<mutex> lock(mtx);
        done = true;
    }
    cv_cons.notify_all();
    c1.join();

    cout << "Producer-Consumer complete!" << endl;
    return 0;
}

/*
    PRODUCER-CONSUMER - Detailed Explanation (Hinglish)
    ====================================================

    YEH PROBLEM KYA HAI:
    - Ek thread (Producer) data banata hai.
    - Doosri thread (Consumer) data use karta hai.
    - Dono ke beech ek shared buffer (queue) hai.

    3 MAIN CONDITIONS handle karni hoti hain:

    1. Buffer FULL hai:
       → Producer ruk jaata hai (cv_prod.wait)
       → Jab consumer kuch nikaale, tab Producer jaagta hai

    2. Buffer EMPTY hai:
       → Consumer ruk jaata hai (cv_cons.wait)
       → Jab producer kuch daale, tab Consumer jaagta hai

    3. Producer ka kaam khatam:
       → done = true set kiya
       → cv_cons.notify_all() se consumer ko jaagaya
       → Consumer empty buffer dekhe aur done=true ho toh exit kare

    IMPORTANT CONCEPTS:
    - condition_variable.wait(lock, predicate):
        Agar predicate FALSE hai → thread sleep ho jaati hai
        Agar predicate TRUE hai → thread aage badhti hai
    - notify_one() → sirf ek waiting thread ko jagao
    - notify_all() → saari waiting threads ko jagao

    FLOW DIAGRAM:
    Producer → buffer mein daala → cv_cons.notify_one()
    Consumer → buffer se nikala → cv_prod.notify_one()
    (Yeh cycle chalti rehti hai jab tak done=true na ho)
*/