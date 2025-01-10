#pragma once

#include <iostream>

#include "../src/board.h"
#include "../src/search.h"

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
inline bool verify_position(const Board &board, const std::string &fen)
{
    int idx = 0;
    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file < 8; file++)
        {
            if (idx >= fen.length())
                return false;

            Piece piece = board.at(rank * 8 + file);
            char expected = fen[idx++];

            if (expected == '.')
            {
                if (piece)
                    return false;
            }
            else
            {
                if (!piece || piece.to_char() != expected)
                    return false;
            }
        }
        idx++;
    }
    return true;
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

inline uint64_t perft(Board &board, int depth, bool print)
{
    if (depth == 0)
        return 1;
    MoveList moves;
    int nr_moves = board.gen_legal_moves<ALL_MOVES>(moves);

    uint64_t nodes = 0;
    for (int i = 0; i < nr_moves; i++)
    {
        Move move = moves[i];
        if (!board.is_legal(move))
            continue;

        board.make_move(move);
        uint64_t x = perft(board, depth - 1, false);
        if (print)
            std::cout << move.to_string() << " : " << x << "\n";
        nodes += x;
        board.undo_move(move);
    }

    return nodes;
}
template <typename Layer> inline bool is_equal_layers(const Layer &a, const Layer &b)
{
    try
    {
        assert(a.num_inputs == b.num_inputs);
        assert(a.num_outputs == b.num_outputs);

        for (int i = 0; i < a.num_inputs; ++i)
            for (int j = 0; j < a.num_outputs; ++j)
                assert(a.weights[i][j] == b.weights[i][j]);

        for (int i = 0; i < a.num_outputs; ++i)
            assert(a.bias[i] == b.bias[i]);
    }
    catch (...)
    {
        return false;
    }
    return true;
}
template <typename T> inline T fast_sigmoid(const T &x)
{
    return x / (1 + std::abs(x));
}
} // namespace BBD::Tests