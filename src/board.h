#pragma once

#include <array>
#include <vector>
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

    // update bitmap for captured piece
    update_bitmap(current_color.flip(), squares[to].type(), to, 0);
    captured_pieces.push_back(squares[to]);

    switch ( move.type() ) {
      case CASTLE:
        // queen's side
        if ( to > from ) {
          update_bitmap(current_color, PieceTypes::KING, to + 1, 0);
          update_bitmap(current_color, PieceTypes::KING, to - 1, 1);
        } else {
          update_bitmap(current_color, PieceTypes::KING, to + 1, 1);
          update_bitmap(current_color, PieceTypes::KING, to - 1, 0);
        }
        std::swap(squares[to + 1], squares[to - 1]);

      case ENPASSANT:
        update_bitmap(current_color.flip(), PieceTypes::PAWN, to - 8, 0);
        captured_pieces.push_back(squares[to-8]);
        squares[to - 8] = Piece();

      case PROMO_KNIGHT:
        update_bitmap(current_color, PieceTypes::PAWN, from, 0);
        captured_pieces.push_back(squares[from]);
        squares[from] = (current_color ? Pieces::WHITE_KNIGHT : Pieces::BLACK_KNIGHT);

      case PROMO_BISHOP:
        update_bitmap(current_color, PieceTypes::PAWN, from, 0);
        captured_pieces.push_back(squares[from]);
        squares[from] = (current_color ? Pieces::WHITE_BISHOP : Pieces::BLACK_BISHOP);

      case PROMO_ROOK:
        update_bitmap(current_color, PieceTypes::PAWN, from, 0);
        captured_pieces.push_back(squares[from]);
        squares[from] = (current_color ? Pieces::WHITE_ROOK : Pieces::BLACK_ROOK);

      case PROMO_QUEEN:
        update_bitmap(current_color, PieceTypes::PAWN, from, 0);
        captured_pieces.push_back(squares[from]);
        squares[from] = (current_color ? Pieces::WHITE_QUEEN : Pieces::BLACK_QUEEN);

      default:break;
    }


    // update the bitmap for moving piece
    update_bitmap(current_color, squares[from].type(), to, 1);
    update_bitmap(current_color, squares[from].type(), from, 0);

    // make move
    std::swap(squares[to], squares[from]);
    squares[from] = Piece();

    current_color = current_color.flip();
    return true;
  };

  /// Updates the Board, assuming the move is legal
  /// \param move
  /// \return
  bool undo_move(const Move& move) {
    Square from = move.from();
    Square to = move.to();

    current_color = current_color.flip();

    switch ( move.type() ) {
      case CASTLE:
        // queen's side
        if ( to > from ) {
          update_bitmap(current_color, PieceTypes::KING, to + 1, 1);
          update_bitmap(current_color, PieceTypes::KING, to - 1, 0);
        } else {
          update_bitmap(current_color, PieceTypes::KING, to + 1, 0);
          update_bitmap(current_color, PieceTypes::KING, to - 1, 1);
        }
        std::swap(squares[to + 1], squares[to - 1]);

      case ENPASSANT:
        update_bitmap(current_color.flip(), PieceTypes::PAWN, to - 8, 1);
        squares[to - 8] = captured_pieces.back();
        captured_pieces.pop_back();

      case PROMO_KNIGHT:
      case PROMO_BISHOP:
      case PROMO_ROOK:
      case PROMO_QUEEN: {
        update_bitmap(current_color, squares[to].type(), to, 0);
        squares[to] = captured_pieces.back();
        captured_pieces.pop_back();
        break;
      }

      default:break;
    }

    update_bitmap(current_color, squares[to].type(), to, 0);
    update_bitmap(current_color, squares[to].type(), from, 1);

    // undo move
    std::swap(squares[to], squares[from]);
    squares[to] = captured_pieces.back();
    captured_pieces.pop_back();
    update_bitmap(current_color.flip(), squares[to].type(), to, 1);

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
  Color current_color = Colors::WHITE;
  // TODO[georgii] : change Piece() to Pieces::EMPTY
  std::vector<Piece> captured_pieces;

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
    if (piece == Pieces::EMPTY)
      return;

    auto& pieces = (color == Colors::BLACK) ? black_pieces : white_pieces;
    if (value)
      pieces[piece] |= (1ULL << index);
    else
      pieces[piece] &= ~(1ULL << index);
  }
};

}
