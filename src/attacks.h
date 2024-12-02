#pragma once
#include <cstdint>
#include "square.h"
#include "color.h"
#define Bitboard uint64_t

using namespace BBD;

namespace BBD::attacks {
    // helper functions
    void print_mask(Bitboard mask) {
        for (uint8_t rank = 0; rank < 8; rank++) {
            for (uint8_t file = 0; file < 8; file++) {
                if (mask & (1ull << (8 * rank + file))) std::cout << "1 ";
                else std::cout << "0 ";
            }
            std::cout << "\n";
        }
        std::cout << "------------------\n";
    }

    bool inside_board(uint8_t rank, uint8_t file) { return 0 <= rank && rank < 8 && 0 <= file && file < 8; }

    std::array<std::array<Bitboard, 64>, 2> pawn_attacks; // colored attacks
    std::array<Bitboard, 64> knight_attacks;
    std::array<Bitboard, 64> king_attacks;

    void init_pawn_attacks() {
        constexpr uint8_t file_a = 0, file_h = 7;
        constexpr uint8_t rank_2 = 1, rank_7 = 6;

        for (Square sq = Squares::A1; sq <= Squares::H8; sq++) {
            const uint8_t file = sq.file(), rank = sq.rank();
            if (file != file_a) {
                if (rank != rank_7)
                    pawn_attacks[Colors::WHITE][sq] |= 1ull << Square(rank + 1, file - 1);
                if (rank != rank_2)
                    pawn_attacks[Colors::BLACK][sq] |= 1ull << Square(rank - 1, file - 1);
            }
            if (file != file_h) {
                if (rank != rank_7)
                    pawn_attacks[Colors::WHITE][sq] |= 1ull << Square(rank + 1, file + 1);
                if (rank != rank_2)
                    pawn_attacks[Colors::BLACK][sq] |= 1ull << Square(rank - 1, file + 1);
            }
        }
    }

    void init_knight_attacks() {
        constexpr std::array<std::pair<int, int>, 8> knight_delta = {
            {-2, -1}, {-2, 1}, {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}
        };

        for (Square sq = Squares::A1; sq <= Squares::H8; sq++) {
            const uint8_t file = sq.file(), rank = sq.rank();

            for (auto [d_rank, d_file] : knight_delta) {
                const uint8_t new_file = file + d_file, new_rank = rank + d_rank;
                if (inside_board(new_rank, new_file)) knight_attacks[sq] |= 1ull << Square(new_rank, new_file);
            }
        }
    }

    void init_king_attacks() {
        constexpr std::array<std::pair<int, int>, 8> king_delta = {
            {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}
        };

        for (Square sq = Squares::A1; sq <= Squares::H8; sq++) {
            const uint8_t file = sq.file(), rank = sq.rank();

            for (auto [d_rank, d_file] : king_delta) {
                const uint8_t new_file = file + d_file, new_rank = rank + d_rank;
                if (inside_board(new_rank, new_file)) king_attacks[sq] |= 1ull << Square(new_rank, new_file);
            }
        }
    }

    void init() {
        init_pawn_attacks();
        init_knight_attacks();
        init_king_attacks();
        //init_slider_attacks();
    }

}; // namespace BBD::attacks
