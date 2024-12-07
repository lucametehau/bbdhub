#pragma once
#include "color.h"
#include <array>
#include <cstdint>

namespace BBD
{

/*
We have 12 pieces, so we will represent the piece types as:
- 0 is PAWN
- 1 is KNIGHT
- 2 is BISHOP
- 3 is ROOK
- 4 is QUEEN
- 5 is KING
The normal piece will be encoded as 2 * piece_type + color.
For example: WHITE_KING = 2 * KING + WHITE = 11.
*/

class PieceType
{
  public:
    uint8_t m_piece_type;

  public:
    constexpr PieceType() = default;
    constexpr PieceType(uint8_t id) : m_piece_type(id)
    {
    }

    char to_char()
    {
        constexpr std::array<char, 6> char_map = std::array{'p', 'n', 'b', 'r', 'q', 'k'};
        return char_map[m_piece_type];
    }

    constexpr bool operator==(const PieceType &other) const
    {
        return m_piece_type == other.m_piece_type;
    }
    constexpr PieceType(const PieceType &other) = default;

    constexpr PieceType &operator=(const PieceType &other) = default;

    operator uint8_t() const
    {
        return m_piece_type;
    }
};

namespace PieceTypes
{
constexpr PieceType PAWN = PieceType(0);
constexpr PieceType KNIGHT = PieceType(1);
constexpr PieceType BISHOP = PieceType(2);
constexpr PieceType ROOK = PieceType(3);
constexpr PieceType QUEEN = PieceType(4);
constexpr PieceType KING = PieceType(5);
}; // namespace PieceTypes

class Piece
{
  private:
    uint8_t m_piece;

  public:
    constexpr Piece() = default;
    constexpr Piece(uint8_t id) : m_piece(id)
    {
    }

    constexpr Piece(const Piece &other) = default;
    constexpr Piece &operator=(const Piece &other) = default;

    PieceType type() const
    {
        return PieceType(m_piece / 2);
    }
    Color color() const
    {
        return m_piece & 1;
    } // TODO: Make color
    operator bool() const
    {
        return m_piece != 255;
    }

    constexpr bool operator==(const Piece &other) const
    {
        return m_piece == other.m_piece;
    }
    char to_char() const
    {
        return color() == Colors::WHITE ? toupper(type().to_char()) : type().to_char(); // TODO: Change to color
    }
    ~Piece() = default;
};

namespace Pieces
{
constexpr Piece NO_PIECE = Piece(-1);
constexpr Piece BLACK_PAWN = Piece(0);
constexpr Piece WHITE_PAWN = Piece(1);
constexpr Piece BLACK_KNIGHT = Piece(2);
constexpr Piece WHITE_KNIGHT = Piece(3);
constexpr Piece BLACK_BISHOP = Piece(4);
constexpr Piece WHITE_BISHOP = Piece(5);
constexpr Piece BLACK_ROOK = Piece(6);
constexpr Piece WHITE_ROOK = Piece(7);
constexpr Piece BLACK_QUEEN = Piece(8);
constexpr Piece WHITE_QUEEN = Piece(9);
constexpr Piece BLACK_KING = Piece(10);
constexpr Piece WHITE_KING = Piece(11);
}; // namespace Pieces

}; // namespace BBD
