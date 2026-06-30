// observers/ConsoleNotifier.h — Concrete observer: game events ko console pe
// print karta hai. IObserver implement karta hai.
#ifndef TICTACTOE_LLD_OBSERVERS_CONSOLENOTIFIER_H
#define TICTACTOE_LLD_OBSERVERS_CONSOLENOTIFIER_H

#include <bits/stdc++.h>

#include "IObserver.h"

using namespace std;

namespace tictactoe_lld {

class ConsoleNotifier : public IObserver {
public:
    void update(const string &message) override {
        cout << "[Notification] " << message << "\n";
    }
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_OBSERVERS_CONSOLENOTIFIER_H
