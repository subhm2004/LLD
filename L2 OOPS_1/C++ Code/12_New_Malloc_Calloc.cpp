// ============================================================================
//  12_New_Malloc_Calloc.cpp  —  new/delete vs malloc/free vs calloc
// ----------------------------------------------------------------------------
//  `new`/`delete` (C++): memory allocate karta hai AUR constructor/destructor
//  chalata hai -> objects ke liye sahi. `malloc`/`free` (C): sirf raw memory
//  deta hai, constructor NAHI chalता. `calloc`: malloc jaisa par memory ko 0 se
//  initialize karta hai. C++ me objects ke liye hamesha new/delete (ya smart
//  pointers) use karo, malloc/calloc nahi.
// ============================================================================
#include <iostream>
#include <cstdlib>
using namespace std;

struct Pod { int x; double y; };

int main() {
    // C++ new — calls constructor, returns typed pointer, throws on fail (nothrow optional)
    Pod* p1 = new Pod{10, 3.14};
    cout << "new Pod: " << p1->x << ", " << p1->y << "\n";
    delete p1;

    // malloc — NO constructor, raw bytes, returns void*
    Pod* p2 = static_cast<Pod*>(malloc(sizeof(Pod)));
    p2->x = 20;
    p2->y = 2.71;
    cout << "malloc Pod (uninitialized ctor): " << p2->x << "\n";
    free(p2);

    // calloc — like malloc but zero-initializes memory
    int* arr = static_cast<int*>(calloc(5, sizeof(int)));
    cout << "calloc arr[0] (zero-init): " << arr[0] << "\n";
    free(arr);

    // new[] / delete[] for arrays
    int* nums = new int[3]{1, 2, 3};
    delete[] nums;

    cout << "\nInterview:\n"
         << "  new/delete  → C++, ctor/dtor, type-safe\n"
         << "  malloc/free → C, no ctor, manual cast\n"
         << "  calloc      → malloc + zero-fill\n";
    return 0;
}
