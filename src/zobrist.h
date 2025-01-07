#pragma once
#include "piece.h"
#include "square.h"
#include <array>
#include <random>

namespace BBD::Zobrist
{

inline std::array<uint64_t, 12 * 64> piece_square_keys;
inline std::array<uint64_t, 4> castling_keys;
inline std::array<uint64_t, 64> en_passant_keys;

inline uint64_t black_to_move;

inline void init()
{
    std::mt19937_64 rng(0xBEEF);
    for (auto &it : piece_square_keys)
        it = rng();
    for (auto &it : castling_keys)
        it = rng();
    for (auto &it : en_passant_keys)
        it = rng();
    black_to_move = rng();
}

}; // namespace BBD::Zobrist
