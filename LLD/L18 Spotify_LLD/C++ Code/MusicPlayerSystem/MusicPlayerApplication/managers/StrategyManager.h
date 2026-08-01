// ============================================================================
//  managers/StrategyManager.h — SINGLETON + strategy provider (Factory-ish)
// ----------------------------------------------------------------------------
//  PlayStrategyType do, sahi PlayStrategy object lo (Sequential/Random/
//  CustomQueue). Ye Factory jaisa hai, par ek KHAAS difference:
//
//  ⭐ Teeno strategies EK BAAR banti hain (constructor me) aur REUSE hoti
//  hain — har baar new nahi. Ye ek FLYWEIGHT/cache flavour hai: strategy
//  objects stateless-ish hain (playlist set hone pe reset ho jaate), to
//  ek hi instance baar-baar kaam aata hai. Memory efficient!
//
//  SINGLETON kyun: strategies ka ek central pool. Facade isse getStrategy()
//  se maangta hai jab user play-mode switch karta hai.
// ============================================================================
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