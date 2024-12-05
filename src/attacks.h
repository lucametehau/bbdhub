#pragma once
#include "bitboard.h"
#include "color.h"
#include "piece.h"
#include "square.h"
#include <array>
#include <cstdint>

namespace BBD::attacks
{
// helper functions
inline bool inside_board(uint8_t rank, uint8_t file)
{
    return 0 <= rank && rank < 8 && 0 <= file && file < 8;
}

inline std::array<std::array<Bitboard, 64>, 2> pawn_attacks; // colored attacks
inline std::array<Bitboard, 64> knight_attacks;
inline std::array<Bitboard, 64> king_attacks;

inline std::array<Bitboard, 8> rank_mask, file_mask;
inline std::array<Bitboard, 15> diagonal_mask, anti_diagonal_mask;

inline void init_pawn_attacks()
{
    constexpr uint8_t file_a = 0, file_h = 7;
    constexpr uint8_t rank_1 = 0, rank_8 = 7;

    for (Square sq = Squares::A1; sq <= Squares::H8; sq++)
    {
        const uint8_t file = sq.file(), rank = sq.rank();
        if (file != file_a)
        {
            if (rank < rank_8)
                pawn_attacks[Colors::WHITE][sq] |= Bitboard(Square(rank + 1, file - 1));
            if (rank > rank_1)
                pawn_attacks[Colors::BLACK][sq] |= Bitboard(Square(rank - 1, file - 1));
        }
        if (file != file_h)
        {
            if (rank < rank_8)
                pawn_attacks[Colors::WHITE][sq] |= Bitboard(Square(rank + 1, file + 1));
            if (rank > rank_1)
                pawn_attacks[Colors::BLACK][sq] |= Bitboard(Square(rank - 1, file + 1));
        }
    }
}

inline Bitboard generate_attacks_pawn(Color color, Square sq)
{
    return pawn_attacks[color][sq];
}

inline void init_knight_attacks()
{
    constexpr std::array<std::pair<int, int>, 8> knight_delta = {
        {{-2, -1}, {-2, 1}, {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}}};

    for (Square sq = Squares::A1; sq <= Squares::H8; sq++)
    {
        const uint8_t file = sq.file(), rank = sq.rank();

        for (auto [d_rank, d_file] : knight_delta)
        {
            const uint8_t new_file = file + d_file, new_rank = rank + d_rank;
            if (inside_board(new_rank, new_file))
                knight_attacks[sq] |= Bitboard(Square(new_rank, new_file));
        }
    }
}

inline void init_king_attacks()
{
    constexpr std::array<std::pair<int, int>, 8> king_delta = {
        {{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}}};

    for (Square sq = Squares::A1; sq <= Squares::H8; sq++)
    {
        const uint8_t file = sq.file(), rank = sq.rank();

        for (auto [d_rank, d_file] : king_delta)
        {
            const uint8_t new_file = file + d_file, new_rank = rank + d_rank;
            if (inside_board(new_rank, new_file))
                king_attacks[sq] |= Bitboard(Square(new_rank, new_file));
        }
    }
}

/*
Ok so this is the most complicated part by far in the attacks generation.
The name is quite grandious, all it is is some bitmask manipulation.
I recommend reading here:
https://www.chessprogramming.org/Hyperbola_Quintessence And here:
https://www.chessprogramming.org/Subtracting_a_Rook_from_a_Blocking_Piece
*/

inline void init_slider_attacks()
{
    for (Square sq = Squares::A1; sq <= Squares::H8; sq++)
    {
        Bitboard sq_mask(sq);
        rank_mask[sq.rank()] |= sq_mask;
        file_mask[sq.file()] |= sq_mask;
        anti_diagonal_mask[sq.rank() + sq.file()] |= sq_mask;
        diagonal_mask[7 + sq.rank() - sq.file()] |= sq_mask;
    }
}

inline Bitboard reverse_bits(Bitboard mask)
{
    return __builtin_bitreverse64(mask);
}

inline Bitboard hyperbola_quintessence(Square sq, Bitboard occ, Bitboard mask)
{
    Bitboard occ_on_mask = occ & mask;
    Bitboard mask1 = occ_on_mask - (Bitboard(sq) << static_cast<int8_t>(1));
    Bitboard mask2 = reverse_bits(reverse_bits(occ_on_mask) - (Bitboard(Square(sq ^ 63)) << static_cast<int8_t>(1)));
    return mask & (mask1 ^ mask2);
}

inline Bitboard generate_attacks_bishop(Square sq, Bitboard occ)
{
    return hyperbola_quintessence(sq, occ, diagonal_mask[7 + sq.rank() - sq.file()]) |
           hyperbola_quintessence(sq, occ, anti_diagonal_mask[sq.rank() + sq.file()]);
}

inline Bitboard generate_attacks_rook(Square sq, Bitboard occ)
{
    return hyperbola_quintessence(sq, occ, rank_mask[sq.rank()]) |
           hyperbola_quintessence(sq, occ, file_mask[sq.file()]);
}

inline Bitboard generate_attacks(PieceType piece_type, Square sq, Bitboard occ)
{
    if (piece_type == PieceTypes::KNIGHT)
        return knight_attacks[sq];
    if (piece_type == PieceTypes::BISHOP)
        return generate_attacks_bishop(sq, occ);
    if (piece_type == PieceTypes::ROOK)
        return generate_attacks_rook(sq, occ);
    if (piece_type == PieceTypes::QUEEN)
        return generate_attacks_bishop(sq, occ) | generate_attacks_rook(sq, occ);
    if (piece_type == PieceTypes::KING)
        return king_attacks[sq];
    assert(0);
    return 0;
}

inline std::array<std::array<Bitboard, 64>, 64> between_mask, line_mask;

inline void init_line_masks()
{

    constexpr std::array<std::pair<int, int>, 8> king_delta = {
        {{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}}};

    for (Square sq = Squares::A1; sq <= Squares::H8; sq++)
    {
        const uint8_t file = sq.file(), rank = sq.rank();

        for (auto [d_rank, d_file] : king_delta)
        {
            uint8_t new_file = file, new_rank = rank;
            Bitboard mask(0ull);
            while (inside_board(new_rank, new_file))
            {
                const Square sq_to = Square(new_rank, new_file);
                between_mask[sq][sq_to] = mask & ~Bitboard(sq);
                mask |= Bitboard(sq_to);
                line_mask[sq][sq_to] = mask;
                new_file += d_file, new_rank += d_rank;
            }
        }
    }
}

inline void init()
{
    init_pawn_attacks();
    init_knight_attacks();
    init_king_attacks();
    init_slider_attacks();
    init_line_masks();
}

}; // namespace BBD::attacks
