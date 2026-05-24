/**
 * Move semantics — rvalue references, std::move
 * Steal resources instead of copying
 */
#include <iostream>
#include <utility>
#include <vector>
using namespace std;

class Buffer {
    int* data;
    size_t n;
public:
    Buffer(size_t size) : n(size) {
        data = new int[n];
        cout << "[Buffer] ctor " << n << " ints\n";
    }

    // Copy
    Buffer(const Buffer& o) : n(o.n) {
        data = new int[n];
        cout << "[Buffer] copy ctor\n";
    }

    // Move — steal pointer
    Buffer(Buffer&& o) noexcept : data(o.data), n(o.n) {
        o.data = nullptr;
        o.n = 0;
        cout << "[Buffer] move ctor\n";
    }

    Buffer& operator=(Buffer&& o) noexcept {
        if (this == &o) return *this;
        delete[] data;
        data = o.data;
        n = o.n;
        o.data = nullptr;
        o.n = 0;
        cout << "[Buffer] move assignment\n";
        return *this;
    }

    ~Buffer() {
        delete[] data;
        cout << "[Buffer] dtor\n";
    }
};

Buffer makeBuffer() {
    return Buffer(1000);  // RVO / move from temporary
}

int main() {
  Buffer b1 = makeBuffer();           // move (or RVO)
  Buffer b2(500);
  Buffer b3 = std::move(b2);          // explicit move — b2 empty
  cout << "b2 moved-from, safe to destroy\n";

  vector<string> v;
  v.push_back(string("hello"));       // move into vector
  return 0;
}
