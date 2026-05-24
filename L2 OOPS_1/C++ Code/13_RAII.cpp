/**
 * RAII — Resource Acquisition Is Initialization
 * Acquire in ctor, release in dtor → exception-safe scope-bound cleanup
 */
#include <bits/stdc++.h>
#include <mutex>
using namespace std;

class FileGuard {
  FILE *fp;

public:
  explicit FileGuard(const char *path, const char *mode) {
    fp = fopen(path, mode);
    if (!fp)
      throw runtime_error("fopen failed");
    cout << "[RAII] file opened\n";
  }
  ~FileGuard() {
    if (fp) {
      fclose(fp);
      cout << "[RAII] file closed (dtor)\n";
    }
  }
  FileGuard(const FileGuard &) = delete;
  FileGuard &operator=(const FileGuard &) = delete;

  void write(const char *msg) {
    if (fp)
      fputs(msg, fp);
  }
};

class LockGuardDemo {
  mutex &m;

public:
  explicit LockGuardDemo(mutex &mtx) : m(mtx) {
    m.lock();
    cout << "[RAII] mutex locked\n";
  }
  ~LockGuardDemo() {
    m.unlock();
    cout << "[RAII] mutex unlocked\n";
  }
};

int main() {
  {
    FileGuard f("raii_demo_tmp.txt", "w");
    f.write("RAII writes here\n");
  } // dtor closes file even if exception later in scope

  mutex mtx;
  {
    LockGuardDemo lock(mtx);
    cout << "critical section\n";
  }
  return 0;
}
