#pragma once
#include "piece.h"
#include "square.h"
namespace BBD
{

/*
A move has some different possible types.
Castle, En Passant and Promotion (of any non Pawn piece).
*/

typedef uint8_t MoveType;

enum MoveTypes : MoveType
{
    NO_TYPE = 0,
    CASTLE,
    ENPASSANT,
    PROMO_KNIGHT = 4,
    PROMO_BISHOP,
    PROMO_ROOK,
    PROMO_QUEEN
};

/*
We can encode a move by the from square, the to square and the type of the move.
This way, all the moves are unique. The encoding is simple:
First 6 bits for the from square, the next 6 bits for the to square and the last
4 for the move type
*/

class Move
{
  private:
    uint16_t m_move;

  public:
    constexpr Move() = default;
    constexpr Move(Square from, Square to, MoveType type) : m_move(from | (to << 6) | (type << 12))
    {
    }

    operator bool() const
    {
        return m_move;
    }

    constexpr Square from() const
    {
        return m_move & 63;
    }
    constexpr Square to() const
    {
        return (m_move >> 6) & 63;
    }
    constexpr MoveType type() const
    {
        return (m_move >> 12) & 7;
    }

    constexpr bool is_promo() const
    {
        return type() & 4;
    }

    constexpr PieceType promotion_piece() const
    {
        assert(is_promo());
        return PieceType((type() & 3) + 1);
    }

    std::string to_string()
    {
        std::string move_str;
        move_str += from().to_string();
        move_str += to().to_string();
        if (is_promo())
            move_str += promotion_piece().to_char();
        return move_str;
    }
};

constexpr Move NULL_MOVE = Move(); // useful later on

}; // namespace BBD