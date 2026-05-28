#include <bits/stdc++.h>

using namespace std;

// NOTE:
// This demo needs C++17 or newer because it uses `inline static` data members.
// Compile with: g++ -std=c++17 20_Inline_Static_Member_Usecase.cpp

/*
Inline static member use-case (C++17+)
--------------------------------------
Problem (old style):
1) Class ke andar:   static int x;   // declaration only
2) Class ke bahar:   int Class::x=0; // definition required

Inline static (new style):
1) Class ke andar hi define + initialize:
   inline static int x = 0;
2) Bahar separate definition ki zarurat nahi.
*/

class Counter {
private:
    // Old style:
    // - Class ke andar sirf declaration hota hai.
    // - Actual definition class ke bahar deni padti hai.
  static int count;

public:
  Counter() { count++; }
  static int getCount() { return count; }
};

// Old style me yeh line mandatory hai (warna linker error)
int Counter::count = 0;
// Wrong/duplicate definition examples (intentionally commented):
// int Counter::count = 0; // duplicate definition -> linker error
// Counter::count = 0;     // yeh definition nahi, sirf assignment syntax hai

class InlineCounter {
private:
    // C++17 inline static style:
    // - class ke andar hi complete definition + initialization.
    // - bahar separate definition ki zarurat nahi.
  inline static int count = 0;

public:
  InlineCounter() { count++; }
  static int getCount() { return count; }
};

int main() {
  Counter a1, a2;
  InlineCounter b1, b2, b3;

  cout << "Counter count: " << Counter::getCount() << "\n";
  cout << "InlineCounter count: " << InlineCounter::getCount() << "\n";

  return 0;
}
