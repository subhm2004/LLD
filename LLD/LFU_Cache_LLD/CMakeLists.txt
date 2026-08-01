cmake_minimum_required(VERSION 3.14)
project(LFU_Cache_LLD LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(lfu_cache_app main.cpp)
target_compile_options(lfu_cache_app PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(lfu_cache_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Threads REQUIRED)
target_link_libraries(lfu_cache_app PRIVATE Threads::Threads)
