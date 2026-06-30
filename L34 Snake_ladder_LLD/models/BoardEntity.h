// models/BoardEntity.h — Snake ya Ladder ka common model: start + end position.
// Snake neeche le jaata (end < start), Ladder upar (end > start).
#ifndef SNAKE_LADDER_LLD_MODELS_BOARDENTITY_H
#define SNAKE_LADDER_LLD_MODELS_BOARDENTITY_H

#include <bits/stdc++.h>
using namespace std;

namespace snake_ladder_lld {

class BoardEntity {
protected:
    int startPosition_;
    int endPosition_;

public:
    BoardEntity(int start, int end) : startPosition_(start), endPosition_(end) {}
    int getStart() const { return startPosition_; }
    int getEnd() const { return endPosition_; }
    virtual string name() = 0;
    virtual void display() = 0;
    virtual ~BoardEntity() {}
};

class Snake : public BoardEntity {
public:
    Snake(int start, int end) : BoardEntity(start, end) {}
    string name() override { return "SNAKE"; }
    void display() override { cout << "Snake: " << startPosition_ << " -> " << endPosition_ << endl; }
};

class Ladder : public BoardEntity {
public:
    Ladder(int start, int end) : BoardEntity(start, end) {}
    string name() override { return "LADDER"; }
    void display() override { cout << "Ladder: " << startPosition_ << " -> " << endPosition_ << endl; }
};
}

#endif
