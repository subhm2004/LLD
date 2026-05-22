/**
 * 01 — Print in Order (LeetCode 1114) overview.
 * Three threads must print: first → second → third.
 */
#include <iostream>
#include <thread>

#include "PrintInOrder.h"

using namespace std;
using namespace print_in_order;

int main() {
    cout << "====== Print in Order (LC 1114) ======\n";
    cout << "Three threads, strict order: first, second, third\n\n";

    PrintInOrder printer;

    thread t1([&]() { printer.first([]() { cout << "first"; }); });
    thread t2([&]() { printer.second([]() { cout << "second"; }); });
    thread t3([&]() { printer.third([]() { cout << "third"; }); });

    t1.join();
    t2.join();
    t3.join();

    cout << "\n\nPattern: chain of condition_variable waits (pipeline).\n";
    return 0;
}
