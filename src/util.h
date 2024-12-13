#pragma once
#include <chrono>
#include <ctime>

namespace BBD::Engine
{

typedef int16_t Score;

constexpr Score INF = 32000;
constexpr Score MATE = 30000;

inline const auto program_start_time = std::chrono::steady_clock::now();

inline std::time_t get_time_since_start()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - program_start_time)
        .count();
}

} // namespace BBD::Engine