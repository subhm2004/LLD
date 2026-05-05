#ifndef SNAKE_LADDER_LLD_OBSERVERS_CONSOLENOTIFIER_H
#define SNAKE_LADDER_LLD_OBSERVERS_CONSOLENOTIFIER_H

#include <bits/stdc++.h>

#include "IObserver.h"

using namespace std;

namespace snake_ladder_lld {
class SnakeAndLadderConsoleNotifier : public IObserver {
public:
    void update(const string &message) override {
        cout << "[NOTIFICATION] " << message << endl;
    }
};
}

#endif
