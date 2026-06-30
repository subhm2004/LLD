// managers/StrategyManager.h — SINGLETON: PlayStrategyType ke hisaab se sahi
// PlayStrategy (Sequential/Random/CustomQueue) deta hai. Play-order ko swap
// karne ka central point.
#ifndef STRATEGY_MANAGER_HPP
#define STRATEGY_MANAGER_HPP
#include<iostream>
#include "../strategies/SequentialPlayStrategy.h"
#include "../strategies/CustomQueueStrategy.h"
#include "../strategies/RandomPlayStrategy.h"
#include "../enums/PlayStrategyType.h"

using namespace std;

class StrategyManager {
private:
    static StrategyManager* instance;
    SequentialPlayStrategy* sequentialStrategy;
    RandomPlayStrategy* randomStrategy;
    CustomQueueStrategy* customQueueStrategy;

    StrategyManager() {
        sequentialStrategy = new SequentialPlayStrategy();
        randomStrategy = new RandomPlayStrategy();
        customQueueStrategy = new CustomQueueStrategy();
    }
public:
    static StrategyManager* getInstance() {
        if (!instance) {
            instance = new StrategyManager();
        }
        return instance;
    }
    PlayStrategy* getStrategy(PlayStrategyType type) {
        if (type == PlayStrategyType::SEQUENTIAL) {
            return sequentialStrategy;
        } else if (type == PlayStrategyType::RANDOM) {
            return randomStrategy;
        } else {
            return customQueueStrategy;
        }
    }
};

StrategyManager* StrategyManager::instance = nullptr;
#endif // STRATEGY_MANAGER_HPP