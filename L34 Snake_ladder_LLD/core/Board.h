// core/Board.h — N x N board: cells + snakes/ladders ki positions. setupBoard()
// ek strategy ke through populate hota hai (Bridge). Position se next position deta.
#ifndef SNAKE_LADDER_LLD_CORE_BOARD_H
#define SNAKE_LADDER_LLD_CORE_BOARD_H

#include <bits/stdc++.h>

#include "../models/BoardEntity.h"

using namespace std;

namespace snake_ladder_lld {
class BoardSetupStrategy;

class Board {
private:
    int size_;
    vector<BoardEntity *> entities_;
    map<int, BoardEntity *> entityByStart_;

public:
    explicit Board(int dimension) : size_(dimension * dimension) {}
    ~Board() { for (auto *entity : entities_) delete entity; }

    int getBoardSize() const { return size_; }
    bool canAddEntity(int position) { return entityByStart_.find(position) == entityByStart_.end(); }

    void addBoardEntity(BoardEntity *entity) {
        if (!canAddEntity(entity->getStart())) return;
        entities_.push_back(entity);
        entityByStart_[entity->getStart()] = entity;
    }

    BoardEntity *getEntity(int position) {
        auto it = entityByStart_.find(position);
        return it == entityByStart_.end() ? nullptr : it->second;
    }

    void setupBoard(BoardSetupStrategy *strategy);

    void display() {
        cout << "\n=== Board Configuration ===" << endl;
        int snakeCount = 0, ladderCount = 0;
        for (auto *entity : entities_) {
            if (entity->name() == "SNAKE") snakeCount++; else ladderCount++;
        }
        cout << "Board Size: " << size_ << " cells" << endl;
        cout << "\nSnakes: " << snakeCount << endl;
        for (auto *entity : entities_) if (entity->name() == "SNAKE") entity->display();
        cout << "\nLadders: " << ladderCount << endl;
        for (auto *entity : entities_) if (entity->name() == "LADDER") entity->display();
        cout << "=========================" << endl;
    }
};
}

#endif
