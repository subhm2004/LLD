#ifndef LFU_CACHE_LLD_CONFIG_CPPSTANDARD_H
#define LFU_CACHE_LLD_CONFIG_CPPSTANDARD_H

// Compile: g++ -std=c++17 -pthread main.cpp -o lfu_cache_app

#if defined(_MSC_VER)
#if _MSVC_LANG < 201703L
#error "LFU_Cache_LLD requires C++17 or later (/std:c++17)"
#endif
#else
#if __cplusplus < 201703L
#error "LFU_Cache_LLD requires C++17 or later (-std=c++17)"
#endif
#endif

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>

namespace lfu_cache_lld {

inline constexpr int kCppStandardYear = 2017;

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_CONFIG_CPPSTANDARD_H
