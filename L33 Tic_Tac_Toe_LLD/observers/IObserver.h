// observers/IObserver.h — OBSERVER interface for game events (start, move, win,
// draw). Game subject hai; concrete observers (console/UI/log) isse implement karte.
#ifndef TICTACTOE_LLD_OBSERVERS_IOBSERVER_H
#define TICTACTOE_LLD_OBSERVERS_IOBSERVER_H

#include <bits/stdc++.h>

using namespace std;

namespace tictactoe_lld {

class IObserver {
public:
    virtual void update(const string &message) = 0;
    virtual ~IObserver() {}
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_OBSERVERS_IOBSERVER_H
