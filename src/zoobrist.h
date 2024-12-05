#pragma once
#include <array>
#include <random>
#include "piece.h"
#include "square.h"

namespace BBD::Zoobrist {
    
    std::array<uint64_t, 12 * 64> piece_square_keys; 
    std::array<uint64_t, 4> castling_keys;
    std::array<uint64_t, 64> en_passant_keys;

    void init() {
        std::mt19937_64 rng(0xBEEF); 
        for (auto& it : piece_square_keys) it = rng();
        for (auto& it : castling_keys) it = rng();
        for (auto& it : en_passant_keys) it = rng();
    }

};
