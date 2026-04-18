#include <bits/stdc++.h>
#include <thread>
#include <mutex>
using namespace std;

void print(string msg, int count)
{
    for (int i = 1; i <= count; i++)
    {
        cout << msg << " " << i << endl;
    }
}
int main()
{   
    thread t1(print, "Hello", 5);
    thread t2(print, "Bye", 5);
    t1.join();
    t2.join();
    // print("Hello", 5);
    // print("Bye", 5);
    return 0;
}