#pragma once
#include <cstdint>
#include <array>

namespace BBD
{

/*
We have 12 pieces, so we will represent the piece types as:
- 0 is PAWN, 1 is KNIGHT, 2 is BISHOP, 3 is ROOK, 4 is QUEEN, 5 is KING
The normal piece will be encoded as 2 * piece_type + color.
For example: WHITE_KING = 2 * KING + WHITE = 11.
*/

class PieceType {
private:
    uint8_t m_piece_type;
public:
    constexpr PieceType() = default;
    constexpr PieceType(uint8_t id) : m_piece_type(id) {}

    char to_char() {
        constexpr std::array<char, 6> char_map = std::array{'p', 'n', 'b', 'r', 'q', 'k'};
        return char_map[m_piece_type];
    }
};

struct PieceTypes {
    static constexpr PieceType PAWN   = PieceType(0);
    static constexpr PieceType KNIGHT = PieceType(1);
    static constexpr PieceType BISHOP = PieceType(2);
    static constexpr PieceType ROOK   = PieceType(3);
    static constexpr PieceType QUEEN  = PieceType(4);
    static constexpr PieceType KING   = PieceType(5);
};

class Piece {
private:
    uint8_t m_piece;
public:
    constexpr Piece() = default;
    constexpr Piece(uint8_t id) : m_piece(id) {}

    PieceType type() const { return PieceType(m_piece / 2); }
    bool color() const { return m_piece & 1; } // TODO: Make color
    operator bool() const { return m_piece; }

    char to_char() const {
        return color() == 1 ? toupper(type().to_char()) : type().to_char(); // TODO: Change to color
    }
};

struct Pieces {
    static constexpr Piece BLACK_PAWN   = Piece(0);
    static constexpr Piece WHITE_PAWN   = Piece(1);
    static constexpr Piece BLACK_KNIGHT = Piece(2);
    static constexpr Piece WHITE_KNIGHT = Piece(3);
    static constexpr Piece BLACK_BISHOP = Piece(4);
    static constexpr Piece WHITE_BISHOP = Piece(5);
    static constexpr Piece BLACK_ROOK   = Piece(6);
    static constexpr Piece WHITE_ROOK   = Piece(7);
    static constexpr Piece BLACK_QUEEN  = Piece(8);
    static constexpr Piece WHITE_QUEEN  = Piece(9);
    static constexpr Piece BLACK_KING   = Piece(10);
    static constexpr Piece WHITE_KING   = Piece(11);
};

}; // namespace BBD
