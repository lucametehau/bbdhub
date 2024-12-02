#pragma once

#include <array>
#include "color.h"
#include "move.h"
#include "piece.h"
#include "square.h"
//#include "bitboard.h"


namespace BBD {

class Board {
 public:

  constexpr Board() = default;

  /// Updates the Board, assuming the move is legal
  /// \param move
  /// \return
  bool make_move(const Move& move) {
    Square from = move.from();
    Square to = move.to();

    update_bitmap(current_color, squares[from].type(), to, 1);
    update_bitmap(current_color, squares[from].type(), from, 0);

    switch ( move.type()) {
      case CASTLE:
        // queen's side
        if ( to > from ) {
          update_bitmap(current_color, PieceTypes::KING, to + 1, 0);
          update_bitmap(current_color, PieceTypes::KING, to - 1, 1);
        } else {
          update_bitmap(current_color, PieceTypes::KING, to + 1, 1);
          update_bitmap(current_color, PieceTypes::KING, to - 1, 0);
        }
        // TODO: check for copy constructor call here
        std::swap(squares[to + 1], squares[to - 1]);

      case ENPASSANT:
        update_bitmap(current_color.flip(), PieceTypes::PAWN, to - 8, 0);
        squares[to - 8] = Piece();

      case PROMO_KNIGHT:
        update_bitmap(current_color, PieceTypes::KNIGHT, to, 1);
        squares[from] = (squares[from].color() ? Pieces::WHITE_KNIGHT : Pieces::BLACK_KNIGHT);

      case PROMO_BISHOP:
        update_bitmap(current_color, PieceTypes::BISHOP, to, 1);
        squares[from] = (squares[from].color() ? Pieces::WHITE_BISHOP : Pieces::BLACK_BISHOP);

      case PROMO_ROOK:
        update_bitmap(current_color, PieceTypes::ROOK, to, 1);
        squares[from] = (squares[from].color() ? Pieces::WHITE_ROOK : Pieces::BLACK_ROOK);

      case PROMO_QUEEN:
        update_bitmap(current_color, PieceTypes::QUEEN, to, 1);
        squares[from] = (squares[from].color() ? Pieces::WHITE_QUEEN : Pieces::BLACK_QUEEN);

      default:break;
    }

    // make move
    // TODO: check for copy constructor call here
    std::swap(squares[to], squares[from]);
    squares[from] = Piece();

    current_color = current_color.flip();
    return true;
  };

  /// Updates the Board, assuming the move is legal
  /// \param move
  /// \return
  bool undo_move(const Move& move) {
    return true;
  };

  /// Returns the piece at square
  /// \param square
  /// \return
  Piece at(uint8_t square) const {
    return squares[square];
  }

  /// Returns the color of the current player
  /// \return
  Color player_color() const {
    return current_color;
  }

 private:
  std::array<Piece, 64> squares;
  std::array<uint64_t, 6> white_pieces;
  std::array<uint64_t, 6> black_pieces;
  Color current_color;

  uint8_t str_to_index(const char* square) const {
    if ( strlen(square) != 2 )
      throw std::runtime_error("Square index is incorrect!");

    char col = square[0];
    char row = square[1];
    if ( col - 'a' >= 8 || row - '1' >= 8 )
      throw std::runtime_error("Square index is incorrect!");

    return (row - '1') * 8 + (col - 'a');
  }

  /// Checks if the move is legal
  /// \param move
  /// \return
  bool is_legal(Move& move) const {
    return true;
  }

  void update_bitmap(const Color& color, uint8_t piece, uint8_t index, uint8_t value) {
    if ( color == Colors::BLACK ) {
      if ( value )
        black_pieces[piece] |= 1 << index;
      else
        black_pieces[piece] &= ~(1 << index);
    } else {
      if ( value )
        white_pieces[piece] |= 1 << index;
      else
        white_pieces[piece] &= ~(1 << index);
    }
  }
};

}
