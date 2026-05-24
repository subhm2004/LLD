#include <iostream>
#include <cstddef>
using namespace std;

struct BadLayout {
    char a;   // 1 byte + 3 padding
    int b;    // 4
    char c;   // 1 + 3 padding
};          // often sizeof = 12 on 64-bit

struct GoodLayout {
    int b;    // 4
    char a;   // 1
    char c;   // 1 + 2 padding to align struct
};          // often sizeof = 8

struct alignas(16) AlignedBlock {
    char x;
    // compiler may pad so sizeof % 16 == 0
};

#pragma pack(push, 1)
struct Packed {
    char a;
    int b;
    char c;
};  // tight packing — may be slower on some CPUs
#pragma pack(pop)

int main() {
    cout << "=== Padding (compiler inserts unused bytes for alignment) ===\n";
    cout << "BadLayout:  sizeof=" << sizeof(BadLayout)
         << "  offsetof(b)=" << offsetof(BadLayout, b)
         << "  offsetof(c)=" << offsetof(BadLayout, c) << endl;
    cout << "GoodLayout: sizeof=" << sizeof(GoodLayout)
         << "  (reorder fields → less waste)\n";

    cout << "\n=== alignof / alignas ===\n";
    cout << "alignof(int)=" << alignof(int)
         << " alignof(double)=" << alignof(double)
         << " alignof(AlignedBlock)=" << alignof(AlignedBlock)
         << " sizeof(AlignedBlock)=" << sizeof(AlignedBlock) << endl;

    cout << "\n=== Greedy alignment (concept) ===\n";
    cout << "Each member aligns to min(its type alignment, struct alignment).\n";
    cout << "Struct size rounds UP to multiple of strictest member alignment.\n";

    cout << "\n=== #pragma pack(1) — Packed sizeof=" << sizeof(Packed) << " ===\n";
    cout << "Trade-off: less memory, possible unaligned access penalty.\n";

    cout << "\nInterview: know sizeof/offsetof, reorder fields, cache-line false sharing.\n";
    return 0;
}
