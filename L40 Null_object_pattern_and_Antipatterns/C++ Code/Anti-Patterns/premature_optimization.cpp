/*
=====================================================================
PREMATURE OPTIMIZATION ANTI-PATTERN
=====================================================================
THEORY:
"Premature optimization is the root of all evil" - Donald Knuth ka
famous quote hai. Iska matlab hai zaroorat se pehle hi code ko
complex, "fast" banane ki koshish karna, jabki abhi uski zaroorat
hi nahi thi.

PROBLEM:
- Code unnecessarily complex ho jata hai
- Readability aur maintainability kam ho jati hai
- Actual performance bottleneck kahin aur hota hai, lekin humne
  wahan optimize kiya jahan zaroorat nahi thi
- Development time waste hota hai

SOLUTION APPROACH:
- Pehle simple, correct code likho
- Profile karo (measure karo) ki actual bottleneck kahan hai
- Sirf tabhi optimize karo jab data se pata chale ki genuinely
  zaroorat hai
=====================================================================
*/

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// ---------------------------------------------------------------
// ANTI-PATTERN VERSION: chhote array ke liye zabardasti complex
// custom sorting algorithm likh diya (yahan simplified counting
// sort dikhaya hai jo sirf chhote range ke numbers ke liye hi
// kaam karega, aur unnecessarily complex hai)
// ---------------------------------------------------------------
class Sorter
{
public:
    void sort(vector<int> &arr)
    {
        if (arr.empty())
            return;

        int maxVal = *max_element(arr.begin(), arr.end());
        int minVal = *min_element(arr.begin(), arr.end());
        int range = maxVal - minVal + 1;

        vector<int> count(range, 0);
        for (int num : arr)
        {
            count[num - minVal]++;
        }

        int index = 0;
        for (int i = 0; i < range; i++)
        {
            while (count[i] > 0)
            {
                arr[index++] = i + minVal;
                count[i]--;
            }
        }
        // Problem: itna complex counting sort likh diya jabki
        // array me sirf 5-10 elements the - std::sort() hi kaafi tha
        // aur zyada readable bhi hota
    }
};

// ---------------------------------------------------------------
// FIXED VERSION: simple, readable, aur built-in optimized function
// use kiya jab tak actual performance data ye na bataye ki custom
// sorting genuinely zaroori hai
// ---------------------------------------------------------------
class SorterFixed
{
public:
    void sort(vector<int> &arr)
    {
        std::sort(arr.begin(), arr.end());
        // Simple aur readable - agar kabhi profiling se pata chale
        // ki ye bottleneck hai, tabhi custom optimization sochenge
    }
};

int main()
{
    vector<int> data1 = {5, 3, 8, 1, 9, 2};
    vector<int> data2 = {5, 3, 8, 1, 9, 2};

    cout << "--- Premature Optimization (Anti-Pattern) Demo ---" << endl;
    Sorter sorter;
    sorter.sort(data1);
    for (int n : data1)
        cout << n << " ";
    cout << endl;

    cout << "\n--- Fixed Version (Simple std::sort) Demo ---" << endl;
    SorterFixed sorterFixed;
    sorterFixed.sort(data2);
    for (int n : data2)
        cout << n << " ";
    cout << endl;

    return 0;
}