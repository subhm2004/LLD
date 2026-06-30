// enums/PlayStrategyType.h — Playback order types: SEQUENTIAL / RANDOM /
// CUSTOM_QUEUE. StrategyManager isse sahi PlayStrategy choose karta hai.
#ifndef PLAY_STRATEGY_TYPE_HPP
#define PLAY_STRATEGY_TYPE_HPP


enum class PlayStrategyType { 
    SEQUENTIAL, 
    RANDOM, 
    CUSTOM_QUEUE 
};

#endif // PLAY_STRATEGY_TYPE_HPP