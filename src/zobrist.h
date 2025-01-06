#pragma once
#include "piece.h"
#include "square.h"
#include <array>
#include <random>

namespace BBD 
{
class Board;  
}

namespace BBD::Zobrist
{
    extern std::array<uint64_t, 12 * 64> piece_square_keys;
    extern std::array<uint64_t, 4> castling_keys;
    extern std::array<uint64_t, 64> en_passant_keys;
    extern uint64_t black_to_move;

    void init();
    uint64_t hash_calc(const BBD::Board &board);
};