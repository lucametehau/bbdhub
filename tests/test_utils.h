#pragma once

#include <iostream>

#include "../src/board.h"

namespace BBD::Tests
{
inline void print_board(const Board &board)
{
    for (int rank = 7; rank >= 0; rank--)
    {
        std::cout << rank + 1 << "  ";
        for (int file = 0; file < 8; file++)
        {
            Piece piece = board.at(rank * 8 + file);
            if (piece != Pieces::NO_PIECE)
            {
                std::cout << piece.to_char() << ' ';
            }
            else
            {
                std::cout << ". ";
            }
        }
        std::cout << '\n';
    }
    std::cout << "\n   a b c d e f g h\n\n";
}
inline bool is_equal(const Board &board1, const Board &board2)
{
    for (Square sq = Squares::A1; sq <= Squares::H8; sq++)
    {
        if (board1.at(sq) != board2.at(sq))
        {
            std::cout << "Mismatch at square " << sq << ": " << board1.at(sq).to_char() << " vs "
                      << board2.at(sq).to_char() << "\n";
            return false;
        }
    }

    if (board1.player_color() != board2.player_color())
    {
        std::cout << "Player color mismatch\n";
        return false;
    }

    if (board1.halfmoves_clock() != board2.halfmoves_clock())
    {
        std::cout << "Half moves mismatch\n";
        return false;
    }

    if (board1.fullmoves_counter() != board2.fullmoves_counter())
    {
        std::cout << "Full moves mismatch\n";
        return false;
    }

    return true;
}
} // namespace BBD::Tests