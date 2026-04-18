#include <bits/stdc++.h>
#include <iostream>
#include <mutex>

using namespace std;

// C++ me mutex help krta h lock and unlock krne k liye (multithreading ka concept hai )
class Singleton
{
private:
    static Singleton *instance;
    static mutex mtx;

    Singleton()
    {
        cout << "Singleton Constructor Called!" << endl;
    }

public:
    static Singleton *getInstance()
    {
        lock_guard<mutex> lock(mtx); // Lock for thread safety (critical section ko lock kr diya hai taki ek baar me ek hi thread critical section me enter kre or wo use execute kare or jab thread vha se bahar nikle to wo critical section ko unlock kr de taki dusri thread us me enter ka paye)
        if (instance == nullptr)
        {
            instance = new Singleton();
        }
        return instance;
    }
};

// Initialize static members
Singleton *Singleton::instance = nullptr;
mutex Singleton::mtx;

int main()
{
    Singleton *s1 = Singleton::getInstance();
    Singleton *s2 = Singleton::getInstance();

    cout << (s1 == s2) << endl;
}