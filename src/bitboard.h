#pragma once
#include <cassert>
#include "square.h"

namespace BBD {
    
    class Bitboard {
    private:
        unsigned long long mask;

    public:
        constexpr Bitboard() = default;
        constexpr Bitboard(Square square) : mask(1ULL << square) { assert(square < 64); }
        constexpr Bitboard(unsigned long long mask) : mask(mask) {}
       
        // Check if a square has a piece
        bool has_square(Square square) const {
            return (mask >> square) & 1;
        }

        constexpr operator unsigned long long() const {
            return mask;
        }

        // Return least significant bit
        unsigned long long lsb() {
            if (mask == 0) return -1;
            return mask & -mask;
        }

        // Return index of least significant bit
        int lsb_index() {
            if (mask == 0) return -1;
            return __builtin_ctzll(mask);
        }

        // Define operators
        Bitboard operator & (const Bitboard &other) const { return mask & other.mask; }
        Bitboard operator | (const Bitboard &other) const { return mask | other.mask; }
        Bitboard operator ^ (const Bitboard &other) const { return mask ^ other.mask; }
        Bitboard operator ~ () const { return ~mask; }
        Bitboard operator << (const int8_t shift) const { return mask << shift; }
        Bitboard operator >> (const int8_t shift) const { return mask >> shift; }

        Bitboard& operator &= (const Bitboard &other) {
            mask &= other.mask;
            return *this;
        }

        Bitboard& operator |= (const Bitboard &other) {
            mask |= other.mask;
            return *this;
        }

        Bitboard& operator ^= (const Bitboard &other) {
            mask ^= other.mask;
            return *this;
        }

        // Print bitboard for debugging
        void print() {
            for (int rank = 7; rank >= 0; rank--) {
                for (int file = 0; file < 8; file++) {
                    if (mask & (1ull << (8 * rank + file))) std::cout << "1 ";
                    else std::cout << "0 ";
                }
            std::cout << "\n";
            }
            std::cout << "------------------\n";
        }
        
    };
}