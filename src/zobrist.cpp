#include "zobrist.h"
#include "board.h" 

namespace BBD::Zobrist
{

std::array<uint64_t, 12 * 64> piece_square_keys;
std::array<uint64_t, 4> castling_keys;
std::array<uint64_t, 64> en_passant_keys;

uint64_t black_to_move;

void init()
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

uint64_t hash_calc(const BBD::Board &board)
{
    uint64_t hash = 0;
    Color current_color = board.player_color();

    if (!current_color)
    { // black
        hash ^= black_to_move;
    }

    // piece-square
    for (Square sq = Squares::A1; sq <= Squares::H8; sq++)
    {
        if (board.at(sq) != Pieces::NO_PIECE)
        {
            uint8_t piece_number = (board.at(sq).type() * 2 + current_color);
            hash ^= piece_square_keys[piece_number * 64 + sq];
        }
    }

    // castling
    // bit 0: WK, bit 1: WQ, bit 2: BK, bit 3: BQ
    uint8_t castling_rights = board.get_castling_rights();
    if (0b0001 & castling_rights)
        hash ^= castling_keys[0];
    if ((1 << 1) & castling_rights)
        hash ^= castling_keys[1];
    if ((1 << 2) & castling_rights)
        hash ^= castling_keys[2];
    if ((1 << 3) & castling_rights)
        hash ^= castling_keys[3];

    // en_passant
    Square en_passant = board.get_en_passant_square();
    if (en_passant != Squares::NO_SQUARE)
    {
        hash ^= en_passant_keys[en_passant];
    }

    return hash;
}

}; // namespace BBD::Zobrist
