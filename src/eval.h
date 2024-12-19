#pragma once 

#include "bitboard.h"
#include "color.h"
#include "move.h"
#include "piece.h"
#include "square.h"
#include "board.h"
#include <array>
#include <vector>


namespace BBD::Engine 
{

int board_evaluation(const Board &board) {
    constexpr int piece_weights[6] = {
        100, // Pawn [0]
        300, // Knight [1]
        300, // Bishop [2]
        500, // Rook  [3]
        900, // Queen [4]
        0,   // King [5]
    };

    int current_score = 0;

    for (PieceType p = PieceTypes::PAWN; p <= PieceTypes::KING; p++) {
        int white_score = board.get_piece_bitboard(Colors::WHITE, p).count();
        int black_score = board.get_piece_bitboard(Colors::BLACK, p).count();

        current_score += (white_score -  black_score) * piece_weights[p];
    }

    return current_score;
} 

}