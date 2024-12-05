#include "../src/board.h"
#include "../src/move.h"
#include <cassert>
#include <iostream>

using namespace BBD;

void print_board(const Board &board) {
  for (int rank = 7; rank >= 0; rank--) {
    std::cout << rank + 1 << "  ";
    for (int file = 0; file < 8; file++) {
      Piece piece = board.at(rank * 8 + file);
      if (piece != Pieces::NO_PIECE) {
        std::cout << piece.to_char() << ' ';
      } else {
        std::cout << ". ";
      }
    }
    std::cout << '\n';
  }
  std::cout << "\n   a b c d e f g h\n\n";
}

bool verify_position(const Board &board, const std::string &fen) {
  int idx = 0;
  for (int rank = 7; rank >= 0; rank--) {
    for (int file = 0; file < 8; file++) {
      if (idx >= fen.length())
        return false;

      Piece piece = board.at(rank * 8 + file);
      char expected = fen[idx++];

      if (expected == '.') {
        if (piece)
          return false;
      } else {
        if (!piece || piece.to_char() != expected)
          return false;
      }
    }
    idx++;
  }
  return true;
}

void test_basic_moves() {
  std::cout << "Testing basic moves...\n";

  Board board;
  std::cout << "Initial position:\n";
  print_board(board);

  Move pawn_move(Squares::E2, Squares::E4, NO_TYPE);
  std::cout << pawn_move.to_string() << " " << pawn_move.from() << " "
            << pawn_move.to() << std::endl;
  board.make_move(pawn_move);
  std::cout << "After e2-e4:\n";
  print_board(board);

  assert(board.at(Squares::E4).type() == PieceTypes::PAWN);
  assert(board.at(Squares::E4).color() == Colors::WHITE);
  assert(!board.at(Squares::E2));

  board.undo_move(pawn_move);
  std::cout << "After undo:\n";
  print_board(board);

  assert(board.at(Squares::E2).type() == PieceTypes::PAWN);
  assert(board.at(Squares::E2).color() == Colors::WHITE);
  assert(!board.at(Squares::E4));

  std::cout << "Basic moves test passed!\n\n";
}

void test_captures() {
  std::cout << "Testing captures...\n";

  Board board;

  Move setup1(Squares::E2, Squares::E4, NO_TYPE);
  Move setup2(Squares::D7, Squares::D5, NO_TYPE);
  board.make_move(setup1);
  board.make_move(setup2);

  std::cout << "Position before capture:\n";
  print_board(board);

  Move capture(Squares::E4, Squares::D5, NO_TYPE);
  board.make_move(capture);

  std::cout << "After capture:\n";
  print_board(board);

  assert(board.at(Squares::D5).type() == PieceTypes::PAWN);
  assert(board.at(Squares::D5).color() == Colors::WHITE);
  assert(!board.at(Squares::E4));

  board.undo_move(capture);
  std::cout << "After undo capture:\n";
  print_board(board);

  assert(board.at(Squares::E4).type() == PieceTypes::PAWN);
  assert(board.at(Squares::E4).color() == Colors::WHITE);
  assert(board.at(Squares::D5).type() == PieceTypes::PAWN);
  assert(board.at(Squares::D5).color() == Colors::BLACK);

  std::cout << "Captures test passed!\n\n";
}

void test_castling() {
  std::cout << "Testing castling...\n";

  Board board;
  print_board(board);

  Move clear1(Squares::E2, Squares::E4, NO_TYPE);
  Move clear2(Squares::G1, Squares::F3, NO_TYPE);
  Move clear3(Squares::F1, Squares::E2, NO_TYPE);
  Move clear4(Squares::D7, Squares::D5, NO_TYPE);
  board.make_move(clear1);
  board.make_move(clear2);
  board.make_move(clear3);
  board.make_move(clear4);

  std::cout << "Position before castle:\n";
  print_board(board);

  Move castle(Squares::H1, Squares::F1, CASTLE);
  board.make_move(castle);

  std::cout << "After castle:\n";
  print_board(board);

  assert(board.at(Squares::G1).type() == PieceTypes::KING);
  assert(board.at(Squares::F1).type() == PieceTypes::ROOK);
  assert(!board.at(Squares::E1));
  assert(!board.at(Squares::H1));

  board.undo_move(castle);
  std::cout << "After undo castle:\n";
  print_board(board);

  assert(board.at(Squares::E1).type() == PieceTypes::KING);
  assert(board.at(Squares::H1).type() == PieceTypes::ROOK);
  assert(!board.at(Squares::F1));
  assert(!board.at(Squares::G1));

  std::cout << "Castling test passed!\n\n";
}

void test_enpassant() {
  std::cout << "Testing castling...\n";

  Board board;
  print_board(board);

  // do white
  Move clear1(Squares::E2, Squares::E5, NO_TYPE);
  Move clear2(Squares::F7, Squares::F5, NO_TYPE);
  Move en_passant(Squares::E5, Squares::F6, ENPASSANT);
  board.make_move(clear1);
  board.make_move(clear2);

  std::cout << "Position before en passant:\n";
  print_board(board);

  board.make_move(en_passant);

  std::cout << "Position after en passant:\n";
  print_board(board);

  assert(board.at(Squares::F6).type() == PieceTypes::PAWN);
  assert(!board.at(Squares::F5));
  assert(!board.at(Squares::E5));

  board.undo_move(en_passant);
  std::cout << "After undo en passant:\n";
  print_board(board);

  assert(board.at(Squares::E5).type() == PieceTypes::PAWN);
  assert(board.at(Squares::F5).type() == PieceTypes::PAWN);
  assert(!board.at(Squares::F6));

  // now do black
  board = Board();
  Move bmove1(Squares::E2, Squares::E4, NO_TYPE);
  Move bmove2(Squares::F7, Squares::F4, NO_TYPE);
  Move bmove3(Squares::G2, Squares::G4, NO_TYPE);
  Move ben_passant(Squares::F4, Squares::E3, ENPASSANT);
  board.make_move(bmove1);
  board.make_move(bmove2);
  board.make_move(bmove3);

  std::cout << "Position before en passant:\n";
  print_board(board);

  board.make_move(ben_passant);

  std::cout << "Position after en passant:\n";
  print_board(board);

  assert(board.at(Squares::E3).type() == PieceTypes::PAWN);
  assert(!board.at(Squares::E4));
  assert(!board.at(Squares::F4));

  board.undo_move(ben_passant);
  std::cout << "After undo en passant:\n";
  print_board(board);

  assert(board.at(Squares::E4).type() == PieceTypes::PAWN);
  assert(board.at(Squares::F4).type() == PieceTypes::PAWN);
  assert(!board.at(Squares::E3));
  std::cout << "Castling test passed!\n\n";
}

void test_promotion() {
  std::cout << "Testing promotion...\n";

  Board board;

  Move setup1(Squares::E2, Squares::E7, NO_TYPE);
  board.make_move(setup1);
  Move black_move(Squares::H7, Squares::H5, NO_TYPE);
  board.make_move(black_move);

  std::cout << "Position before promotion:\n";
  print_board(board);

  Move promote(Squares::E7, Squares::E8, PROMO_QUEEN);
  board.make_move(promote);

  std::cout << "After promotion:\n";
  print_board(board);

  assert(board.at(Squares::E8).type() == PieceTypes::QUEEN);
  assert(board.at(Squares::E8).color() == Colors::WHITE);
  assert(!board.at(Squares::E7));

  board.undo_move(promote);
  std::cout << "After undo promotion:\n";
  print_board(board);

  assert(board.at(Squares::E7).type() == PieceTypes::PAWN);
  assert(board.at(Squares::E7).color() == Colors::WHITE);

  std::cout << "Promotion test passed!\n\n";
}

int main() {
  std::cout << "Starting board tests...\n\n";

  test_basic_moves();
  test_captures();
  test_castling();
  test_enpassant();
  test_promotion();

  std::cout << "All tests passed!\n";
  return 0;
}