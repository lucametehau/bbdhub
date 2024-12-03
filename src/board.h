#pragma once

#include <array>
#include <vector>
#include "bitboard.h"
#include "color.h"
#include "move.h"
#include "piece.h"
#include "square.h"


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
    update_bitmap(current_color.flip(), squares[to].type(), to, false);
    captured_pieces.push_back(squares[to]);

    switch ( move.type() ) {
      case CASTLE:
        // queen's side
        if ( to > from ) {
          update_bitmap(current_color, PieceTypes::KING, to + 1, false);
          update_bitmap(current_color, PieceTypes::KING, to - 1, true);
        } else {
          update_bitmap(current_color, PieceTypes::KING, to + 1, true);
          update_bitmap(current_color, PieceTypes::KING, to - 1, false);
        }
        std::swap(squares[to + 1], squares[to - 1]);

      case ENPASSANT:
        update_bitmap(current_color.flip(), PieceTypes::PAWN, to - 8, false);
        captured_pieces.push_back(squares[to-8]);
        squares[to - 8] = Pieces::EMPTY;

      case PROMO_KNIGHT:
        update_bitmap(current_color, PieceTypes::PAWN, from, false);
        captured_pieces.push_back(squares[from]);
        squares[from] = (current_color ? Pieces::WHITE_KNIGHT : Pieces::BLACK_KNIGHT);

      case PROMO_BISHOP:
        update_bitmap(current_color, PieceTypes::PAWN, from, false);
        captured_pieces.push_back(squares[from]);
        squares[from] = (current_color ? Pieces::WHITE_BISHOP : Pieces::BLACK_BISHOP);

      case PROMO_ROOK:
        update_bitmap(current_color, PieceTypes::PAWN, from, true);
        captured_pieces.push_back(squares[from]);
        squares[from] = (current_color ? Pieces::WHITE_ROOK : Pieces::BLACK_ROOK);

      case PROMO_QUEEN:
        update_bitmap(current_color, PieceTypes::PAWN, from, false);
        captured_pieces.push_back(squares[from]);
        squares[from] = (current_color ? Pieces::WHITE_QUEEN : Pieces::BLACK_QUEEN);

      default:break;
    }


    // update the bitmap for moving piece
    update_bitmap(current_color, squares[from].type(), to, true);
    update_bitmap(current_color, squares[from].type(), from, false);

    // make move
    std::swap(squares[to], squares[from]);
    squares[from] = Pieces::EMPTY;

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
          update_bitmap(current_color, PieceTypes::KING, to + 1, true);
          update_bitmap(current_color, PieceTypes::KING, to - 1, false);
        } else {
          update_bitmap(current_color, PieceTypes::KING, to + 1, false);
          update_bitmap(current_color, PieceTypes::KING, to - 1, true);
        }
        std::swap(squares[to + 1], squares[to - 1]);

      case ENPASSANT:
        update_bitmap(current_color.flip(), PieceTypes::PAWN, to - 8, true);
        squares[to - 8] = captured_pieces.back();
        captured_pieces.pop_back();

      case PROMO_KNIGHT:
      case PROMO_BISHOP:
      case PROMO_ROOK:
      case PROMO_QUEEN: {
        update_bitmap(current_color, squares[to].type(), to, true);
        squares[to] = captured_pieces.back();
        captured_pieces.pop_back();
        break;
      }

      default:break;
    }

    update_bitmap(current_color, squares[to].type(), to, false);
    update_bitmap(current_color, squares[to].type(), from, true);

    // undo move
    std::swap(squares[to], squares[from]);
    squares[to] = captured_pieces.back();
    captured_pieces.pop_back();
    update_bitmap(current_color.flip(), squares[to].type(), to, true);

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
    // TODO[geo_kuz]: Implement move validation
    return current_color;
  }

 private:
  std::array<Piece, 64> squares{};
  std::array<Bitboard, 6> white_pieces{};
  std::array<Bitboard, 6> black_pieces{};
  Color current_color = Colors::WHITE;
  std::vector<Piece> captured_pieces;


  /// Checks if the move is legal
  /// \param move
  /// \return
  bool is_legal(const Move& move) const {
    return true;
  }

  void update_bitmap(const Color& color, PieceType piece, Square index, bool value) {
    if (piece == Pieces::EMPTY)
      return;

    auto& pieces = (color == Colors::BLACK) ? black_pieces : white_pieces;
    if (value)
      pieces[piece] = pieces[piece] | Bitboard(index);
    else
      pieces[piece] = pieces[piece] & ~Bitboard(index);
  }
};

}
