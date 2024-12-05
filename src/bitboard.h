#pragma once
#include "square.h"
#include <cassert>

namespace BBD
{

class Bitboard
{
  private:
    unsigned long long mask;

  public:
    constexpr Bitboard() = default;
    constexpr Bitboard(Square square) : mask(1ULL << square)
    {
        assert(square < 64);
    }
    constexpr Bitboard(unsigned long long mask) : mask(mask)
    {
    }

    // Check if a square has a piece
    bool has_square(Square square) const
    {
        return (mask >> square) & 1;
    }

    constexpr operator unsigned long long() const
    {
        return mask;
    }

    // Return least significant bit
    const unsigned long long lsb() const
    {
        if (mask == 0)
            return -1;
        return mask & -mask;
    }

    // Return index of least significant bit
    const int lsb_index() const
    {
        if (mask == 0)
            return -1;
        return __builtin_ctzll(mask);
    }

    uint8_t count() const
    {
        return __builtin_popcountll(mask);
    }

    void set_bit(Square index, bool value)
    {
        if (value)
        {
            mask |= Bitboard(index);
        }
        else
        {
            mask &= ~(1ULL << index);
        }
    }

    // Define operators
    Bitboard operator&(const Bitboard &other) const
    {
        return mask & other.mask;
    }
    Bitboard operator|(const Bitboard &other) const
    {
        return mask | other.mask;
    }
    Bitboard operator^(const Bitboard &other) const
    {
        return mask ^ other.mask;
    }
    Bitboard operator~() const
    {
        return ~mask;
    }
    Bitboard operator<<(const int8_t shift) const
    {
        return mask << shift;
    }
    Bitboard operator>>(const int8_t shift) const
    {
        return mask >> shift;
    }

    Bitboard &operator&=(const Bitboard &other)
    {
        mask &= other.mask;
        return *this;
    }

    Bitboard &operator|=(const Bitboard &other)
    {
        mask |= other.mask;
        return *this;
    }

    Bitboard &operator^=(const Bitboard &other)
    {
        mask ^= other.mask;
        return *this;
    }

    template <Direction dir> constexpr Bitboard shift(Color color) const
    {
        if (color == Colors::BLACK)
            return dir > 0 ? mask >> dir : mask << static_cast<int8_t>(-dir);
        return dir > 0 ? mask << dir : mask >> static_cast<int8_t>(-dir);
    }

    // Print bitboard for debugging
    void print()
    {
        for (int rank = 7; rank >= 0; rank--)
        {
            for (int file = 0; file < 8; file++)
            {
                if (mask & (1ull << (8 * rank + file)))
                    std::cout << "1 ";
                else
                    std::cout << "0 ";
            }
            std::cout << "\n";
        }
        std::cout << "------------------\n";
    }
};
} // namespace BBD