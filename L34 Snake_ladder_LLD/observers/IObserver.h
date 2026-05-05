#ifndef SNAKE_LADDER_LLD_OBSERVERS_IOBSERVER_H
#define SNAKE_LADDER_LLD_OBSERVERS_IOBSERVER_H

#include <bits/stdc++.h>
using namespace std;

namespace snake_ladder_lld {
class IObserver {
public:
    virtual void update(const string &message) = 0;
    virtual ~IObserver() {}
};
}

#endif
