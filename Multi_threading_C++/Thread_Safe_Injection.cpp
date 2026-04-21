#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
using namespace std;

vector<int> sharedList;
mutex mtx;

void injectData(int threadId, int value)
{
    lock_guard<mutex> lock(mtx); // Lock liya
    sharedList.push_back(value);
    cout << "Thread " << threadId << " ne inject kiya: " << value << endl;
}

int main()
{
    thread t1(injectData, 1, 100);
    thread t2(injectData, 2, 200);
    thread t3(injectData, 3, 300);

    t1.join();
    t2.join();
    t3.join();

    cout << "\nFinal List: ";
    for (int x : sharedList)
        cout << x << " ";
    cout << endl;
    return 0;
}

/*
    THREAD SAFE INJECTION - Detailed Explanation (Hinglish)
    =========================================================

    PROBLEM:
    - Jab multiple threads ek saath ek shared resource (jaise vector) mein
      data daalne ki koshish karte hain,
      toh data corrupt ho sakta hai — isse "data race" kehte hain.

    SOLUTION - lock_guard use kiya:
    - lock_guard ek RAII wrapper hai mutex ka.
    - Jaise hi function call hota hai, lock_guard automatically mutex lock kar leta hai.
    - Function khatam hote hi (scope end pe) automatically unlock bhi ho jaata hai.
    - Isliye koi bhi 2 threads ek saath sharedList ko touch nahi kar sakti.

    FLOW:
    t1 aaya → mutex lock kiya → 100 daala → unlock kiya
    t2 aaya → mutex lock kiya → 200 daala → unlock kiya
    t3 aaya → mutex lock kiya → 300 daala → unlock kiya

    KEY POINT:
    - Bina mutex ke, teenon threads ek saath vector mein ghus sakti thi
      aur memory corrupt ho jaati.
    - lock_guard ne ensure kiya ki ek time pe sirf EK thread inject kare.
*/