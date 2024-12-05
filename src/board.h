#pragma once

#include "bitboard.h"
#include "color.h"
#include "move.h"
#include "piece.h"
#include "square.h"
#include <array>
#include <vector>

namespace BBD
{

class Board
{
  public:
    constexpr Board()
    {
        captured_pieces.reserve(300);
        squares.fill(Pieces::NO_PIECE);

        // white
        squares[Squares::A1] = Pieces::WHITE_ROOK;
        squares[Squares::B1] = Pieces::WHITE_KNIGHT;
        squares[Squares::C1] = Pieces::WHITE_BISHOP;
        squares[Squares::D1] = Pieces::WHITE_QUEEN;
        squares[Squares::E1] = Pieces::WHITE_KING;
        squares[Squares::F1] = Pieces::WHITE_BISHOP;
        squares[Squares::G1] = Pieces::WHITE_KNIGHT;
        squares[Squares::H1] = Pieces::WHITE_ROOK;

        for (Square sq = Squares::A2; sq <= Squares::H2; sq++)
        {
            squares[sq] = Pieces::WHITE_PAWN;
        }

        // black
        squares[Squares::A8] = Pieces::BLACK_ROOK;
        squares[Squares::B8] = Pieces::BLACK_KNIGHT;
        squares[Squares::C8] = Pieces::BLACK_BISHOP;
        squares[Squares::D8] = Pieces::BLACK_QUEEN;
        squares[Squares::E8] = Pieces::BLACK_KING;
        squares[Squares::F8] = Pieces::BLACK_BISHOP;
        squares[Squares::G8] = Pieces::BLACK_KNIGHT;
        squares[Squares::H8] = Pieces::BLACK_ROOK;

        for (Square sq = Squares::A7; sq <= Squares::H7; sq++)
        {
            squares[sq] = Pieces::BLACK_PAWN;
        }

        // bitmaps
        for (Square sq = 0; sq < 64; sq++)
        {
            if (!squares[sq].color())
                continue;
            if (squares[sq].color() == Colors::WHITE)
            {
                pieces[Colors::WHITE][squares[sq].type()].set_bit(sq, true);
            }
            else
            {
                pieces[Colors::BLACK][squares[sq].type()].set_bit(sq, true);
            }
        }

        current_color = Colors::WHITE;
    };

    /// Updates the Board, assuming the move is legal
    /// \param move
    /// \return
    bool make_move(const Move &move)
    {
        Square from = move.from();
        Square to = move.to();

        // update bitmap for captured piece
        if (squares[to] != Pieces::NO_PIECE)
            pieces[current_color.flip()][squares[to].type()].set_bit(to, false);
        captured_pieces.push_back(squares[to]);

        switch (move.type())
        {
        case CASTLE:
            // queen's side
            if (to > from)
            {
                pieces[current_color][PieceTypes::KING].set_bit(to + 1, false);
                pieces[current_color][PieceTypes::KING].set_bit(to - 1, true);
            }
            else
            {
                pieces[current_color][PieceTypes::KING].set_bit(to + 1, true);
                pieces[current_color][PieceTypes::KING].set_bit(to - 1, false);
            }
            std::swap(squares[to + 1], squares[to - 1]);
            break;

        case ENPASSANT: {
            auto to_pos = to + 8 - 16 * current_color;
            pieces[current_color.flip()][PieceTypes::PAWN].set_bit(to_pos, false);
            captured_pieces.push_back(squares[to_pos]);
            squares[to_pos] = Pieces::NO_PIECE;
            break;
        }
        case PROMO_KNIGHT:
        case PROMO_BISHOP:
        case PROMO_ROOK:
        case PROMO_QUEEN:
            pieces[current_color][PieceTypes::PAWN].set_bit(from, false);
            captured_pieces.push_back(squares[from]);
            squares[from] = (current_color ? Piece(2 * move.promotion_piece() + 1) : Piece(2 * move.promotion_piece()));
            pieces[current_color][move.promotion_piece()].set_bit(to, true);
            break;
        default:
            break;
        }

        // update the bitmap for moving piece
        pieces[current_color][squares[from].type()].set_bit(from, false);
        pieces[current_color][squares[from].type()].set_bit(to, true);

        // make move
        std::swap(squares[to], squares[from]);
        squares[from] = Pieces::NO_PIECE;

        current_color = current_color.flip();
        return true;
    };

    /// Updates the Board, assuming the move is legal
    /// \param move
    /// \return
    bool undo_move(const Move &move)
    {
        Square from = move.from();
        Square to = move.to();

        current_color = current_color.flip();

        switch (move.type())
        {
        case CASTLE:
            // queen's side
            if (to > from)
            {
                pieces[current_color][PieceTypes::KING].set_bit(to + 1, true);
                pieces[current_color][PieceTypes::KING].set_bit(to - 1, false);
            }
            else
            {
                pieces[current_color][PieceTypes::KING].set_bit(to + 1, false);
                pieces[current_color][PieceTypes::KING].set_bit(to - 1, true);
            }
            std::swap(squares[to + 1], squares[to - 1]);
            break;
        case ENPASSANT: {
            auto to_pos = to + 8 - 16 * current_color;
            pieces[current_color.flip()][PieceTypes::PAWN].set_bit(to_pos, true);
            squares[to_pos] = captured_pieces.back();
            captured_pieces.pop_back();
            break;
        }
        case PROMO_KNIGHT:
        case PROMO_BISHOP:
        case PROMO_ROOK:
        case PROMO_QUEEN:
            pieces[current_color][squares[to].type()].set_bit(to, true);
            squares[to] = captured_pieces.back();
            captured_pieces.pop_back();
            break;

        default:
            break;
        }
        pieces[current_color][squares[to].type()].set_bit(to, false);
        pieces[current_color][squares[to].type()].set_bit(from, true);

        // undo move
        std::swap(squares[to], squares[from]);
        squares[to] = captured_pieces.back();
        captured_pieces.pop_back();
        if (squares[to] != Pieces::NO_PIECE)
            pieces[current_color.flip()][squares[to].type()].set_bit(to, true);

        return true;
    };

    /// Returns the piece at square
    /// \param square
    /// \return
    Piece at(Square square) const
    {
        return squares[square];
    }

    /// Returns the color of the current player
    /// \return
    Color player_color() const
    {
        // TODO[geo_kuz]: Implement move validation
        return current_color;
    }

  private:
    std::array<Piece, 64> squares;
    std::array<std::array<Bitboard, 6>, 2> pieces;
    Color current_color;
    std::vector<Piece> captured_pieces;

    /// Checks if the move is legal
    /// \param move
    /// \return
    bool is_legal(const Move &move) const
    {
        return true;
    }
};
} // namespace BBD