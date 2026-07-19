// ============================================================================
//  enums/PlayStrategyType.h — Playback order: SEQUENTIAL / RANDOM / CUSTOM_QUEUE
// ----------------------------------------------------------------------------
//  StrategyManager isse sahi PlayStrategy choose karta hai (user ka play-mode
//  button). SEQUENTIAL = normal, RANDOM = shuffle, CUSTOM_QUEUE = "play next".
//  `enum class` type-safe. Naya mode (jaise REPEAT_ONE) = ek value + nayi
//  strategy class — baaki system untouched (Strategy pattern ka fayda).
// ============================================================================
#ifndef PLAY_STRATEGY_TYPE_HPP
#define PLAY_STRATEGY_TYPE_HPP


enum class PlayStrategyType { 
    SEQUENTIAL, 
    RANDOM, 
    CUSTOM_QUEUE 
};

#endif // PLAY_STRATEGY_TYPE_HPP