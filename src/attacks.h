#pragma once
#include "bitboard.h"
#include "color.h"
#include "piece.h"
#include "square.h"
#include <array>
#include <cstdint>

using namespace BBD;

namespace BBD::attacks {
// helper functions
bool inside_board(uint8_t rank, uint8_t file) {
  return 0 <= rank && rank < 8 && 0 <= file && file < 8;
}

std::array<std::array<Bitboard, 64>, 2> pawn_attacks; // colored attacks
std::array<Bitboard, 64> knight_attacks;
std::array<Bitboard, 64> king_attacks;

std::array<Bitboard, 8> rank_mask, file_mask;
std::array<Bitboard, 15> diagonal_mask, anti_diagonal_mask;

void init_pawn_attacks() {
  constexpr uint8_t file_a = 0, file_h = 7;
  constexpr uint8_t rank_1 = 0, rank_8 = 7;

  for (Square sq = Squares::A1; sq <= Squares::H8; sq++) {
    const uint8_t file = sq.file(), rank = sq.rank();
    if (file != file_a) {
      if (rank < rank_8)
        pawn_attacks[Colors::WHITE][sq] |= Bitboard(Square(rank + 1, file - 1));
      if (rank > rank_1)
        pawn_attacks[Colors::BLACK][sq] |= Bitboard(Square(rank - 1, file - 1));
    }
    if (file != file_h) {
      if (rank < rank_8)
        pawn_attacks[Colors::WHITE][sq] |= Bitboard(Square(rank + 1, file + 1));
      if (rank > rank_1)
        pawn_attacks[Colors::BLACK][sq] |= Bitboard(Square(rank - 1, file + 1));
    }
  }
}

Bitboard generate_attacks_pawn(Color color, Square sq) {
  return pawn_attacks[color][sq];
}

void init_knight_attacks() {
  constexpr std::array<std::pair<int, int>, 8> knight_delta = {
      {{-2, -1}, {-2, 1}, {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}}};

  for (Square sq = Squares::A1; sq <= Squares::H8; sq++) {
    const uint8_t file = sq.file(), rank = sq.rank();

    for (auto [d_rank, d_file] : knight_delta) {
      const uint8_t new_file = file + d_file, new_rank = rank + d_rank;
      if (inside_board(new_rank, new_file))
        knight_attacks[sq] |= Bitboard(Square(new_rank, new_file));
    }
  }
}

void init_king_attacks() {
  constexpr std::array<std::pair<int, int>, 8> king_delta = {
      {{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}}};

  for (Square sq = Squares::A1; sq <= Squares::H8; sq++) {
    const uint8_t file = sq.file(), rank = sq.rank();

    for (auto [d_rank, d_file] : king_delta) {
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

void init_slider_attacks() {
  for (Square sq = Squares::A1; sq <= Squares::H8; sq++) {
    Bitboard sq_mask(sq);
    rank_mask[sq.rank()] |= sq_mask;
    file_mask[sq.file()] |= sq_mask;
    anti_diagonal_mask[sq.rank() + sq.file()] |= sq_mask;
    diagonal_mask[7 + sq.rank() - sq.file()] |= sq_mask;
  }
  rank_mask[0].print();
}

Bitboard reverse_bits(Bitboard mask) { return __builtin_bitreverse64(mask); }

Bitboard hyperbola_quintessence(Square sq, Bitboard occ, Bitboard mask) {
  Bitboard occ_on_mask = occ & mask;
  Bitboard mask1 = occ_on_mask - (Bitboard(sq) << static_cast<int8_t>(1));
  Bitboard mask2 =
      reverse_bits(reverse_bits(occ_on_mask) -
                   (Bitboard(Square(sq ^ 63)) << static_cast<int8_t>(1)));
  return mask & (mask1 ^ mask2);
}

Bitboard generate_attacks_bishop(Square sq, Bitboard occ) {
  return hyperbola_quintessence(sq, occ,
                                diagonal_mask[7 + sq.rank() - sq.file()]) |
         hyperbola_quintessence(sq, occ,
                                anti_diagonal_mask[sq.rank() + sq.file()]);
}

Bitboard generate_attacks_rook(Square sq, Bitboard occ) {
  return hyperbola_quintessence(sq, occ, rank_mask[sq.rank()]) |
         hyperbola_quintessence(sq, occ, file_mask[sq.file()]);
}

template <PieceType piece_type>
Bitboard generate_attacks(Square sq, Bitboard occ) {
  if constexpr (piece_type == PieceTypes::KNIGHT)
    return knight_attacks[sq];
  if constexpr (piece_type == PieceTypes::BISHOP)
    return generate_attacks_bishop(sq, occ);
  if constexpr (piece_type == PieceTypes::ROOK)
    return generate_attacks_rook(sq, occ);
  if constexpr (piece_type == PieceTypes::QUEEN)
    return generate_attacks_bishop(sq, occ) | generate_attacks_rook(sq, occ);
  if constexpr (piece_type == PieceTypes::KING)
    return king_attacks[sq];
  assert(0);
  return 0;
}

void init() {
  init_pawn_attacks();
  init_knight_attacks();
  init_king_attacks();
  init_slider_attacks();
}

}; // namespace BBD::attacks
