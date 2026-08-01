#ifndef MULTI_THREADING_PRINT_FOO_BAR_H
#define MULTI_THREADING_PRINT_FOO_BAR_H

#include <functional>
#include <mutex>

#include "../common/CountingSemaphore.h"

namespace print_foo_bar {

/** LeetCode 1115 — print "foo" and "bar" alternately, n times each. */
class FooBar {
public:
    explicit FooBar(int n) : n_(n) {}

    void foo(std::function<void()> printFoo) {
        for (int i = 0; i < n_; ++i) {
            foo_turn_.acquire();
            printFoo();
            bar_turn_.release();
        }
    }

    void bar(std::function<void()> printBar) {
        for (int i = 0; i < n_; ++i) {
            bar_turn_.acquire();
            printBar();
            foo_turn_.release();
        }
    }

private:
    int n_;
    interview_common::CountingSemaphore foo_turn_{1};
    interview_common::CountingSemaphore bar_turn_{0};
};

}  // namespace print_foo_bar

#endif  // MULTI_THREADING_PRINT_FOO_BAR_H
