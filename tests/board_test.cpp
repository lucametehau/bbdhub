#include <gtest/gtest.h>

#include "test_utils.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Tests;

class BoardTest : public ::testing::Test
{
  protected:
    Board board;

    void SetUp() override
    {
        board = Board();
    }
};

TEST_F(BoardTest, NullMove)
{
    board.make_move(NULL_MOVE);
    board.make_move(NULL_MOVE);

    board.undo_null_move();
    board.undo_null_move();
    Board new_board;
    EXPECT_TRUE(is_equal(board, new_board));
}

TEST_F(BoardTest, BasicMoves)
{
    Move pawn_move(Squares::E2, Squares::E4, NO_TYPE);
    board.make_move(pawn_move);

    EXPECT_EQ(board.at(Squares::E4), Pieces::WHITE_PAWN);
    EXPECT_FALSE(board.at(Squares::E2));

    EXPECT_EQ(board.halfmoves_clock(), 0);
    EXPECT_EQ(board.fullmoves_counter(), 0);

    board.undo_move(pawn_move);

    EXPECT_EQ(board.at(Squares::E2), Pieces::WHITE_PAWN);
    EXPECT_FALSE(board.at(Squares::E4));
    EXPECT_EQ(board.halfmoves_clock(), 0);
    EXPECT_EQ(board.fullmoves_counter(), 0);
}

TEST_F(BoardTest, Captures)
{
    Move setup1(Squares::E2, Squares::E4, NO_TYPE);
    Move setup2(Squares::D7, Squares::D5, NO_TYPE);
    board.make_move(setup1);
    board.make_move(setup2);

    EXPECT_EQ(board.halfmoves_clock(), 0);
    EXPECT_EQ(board.fullmoves_counter(), 1);

    Move capture(Squares::E4, Squares::D5, NO_TYPE);
    board.make_move(capture);

    EXPECT_EQ(board.at(Squares::D5), Pieces::WHITE_PAWN);
    EXPECT_FALSE(board.at(Squares::E4));

    EXPECT_EQ(board.halfmoves_clock(), 0);
    EXPECT_EQ(board.fullmoves_counter(), 1);

    board.undo_move(capture);

    EXPECT_EQ(board.at(Squares::E4), Pieces::WHITE_PAWN);
    EXPECT_EQ(board.at(Squares::D5), Pieces::BLACK_PAWN);
}

TEST_F(BoardTest, Castling)
{
    Move clear1(Squares::E2, Squares::E4, NO_TYPE);
    Move clear2(Squares::G1, Squares::F3, NO_TYPE);
    Move clear3(Squares::F1, Squares::E2, NO_TYPE);
    Move clear4(Squares::D7, Squares::D5, NO_TYPE);
    board.make_move(clear1);
    board.make_move(clear2);
    board.make_move(clear3);
    board.make_move(clear4);

    Move castle(Squares::E1, Squares::G1, CASTLE);
    board.make_move(castle);

    EXPECT_EQ(board.at(Squares::G1), Pieces::WHITE_KING);
    EXPECT_EQ(board.at(Squares::F1), Pieces::WHITE_ROOK);

    EXPECT_FALSE(board.at(Squares::E1));
    EXPECT_FALSE(board.at(Squares::H1));

    board.undo_move(castle);

    EXPECT_EQ(board.at(Squares::E1), Pieces::WHITE_KING);
    EXPECT_EQ(board.at(Squares::H1), Pieces::WHITE_ROOK);
    EXPECT_FALSE(board.at(Squares::F1));
    EXPECT_FALSE(board.at(Squares::G1));
}

TEST_F(BoardTest, WhiteEnPassant)
{
    Move clear1(Squares::E2, Squares::E5, NO_TYPE);
    Move clear2(Squares::F7, Squares::F5, NO_TYPE);
    Move en_passant(Squares::E5, Squares::F6, ENPASSANT);
    board.make_move(clear1);
    board.make_move(clear2);

    board.make_move(en_passant);

    EXPECT_EQ(board.at(Squares::F6), Pieces::WHITE_PAWN);

    EXPECT_FALSE(board.at(Squares::F5));
    EXPECT_FALSE(board.at(Squares::E5));

    board.undo_move(en_passant);
    print_board(board);

    EXPECT_EQ(board.at(Squares::F5), Pieces::BLACK_PAWN);
    EXPECT_EQ(board.at(Squares::E5), Pieces::WHITE_PAWN);
    EXPECT_FALSE(board.at(Squares::F6));
}
TEST_F(BoardTest, BlackEnPassant)
{
    Move clear1(Squares::G2, Squares::G4, NO_TYPE);
    Move clear2(Squares::F7, Squares::F4, NO_TYPE);
    Move clear3(Squares::E2, Squares::E4, NO_TYPE);
    Move en_passant(Squares::F4, Squares::E3, ENPASSANT);
    board.make_move(clear1);
    board.make_move(clear2);
    board.make_move(clear3);

    board.make_move(en_passant);

    EXPECT_EQ(board.at(Squares::E3), Pieces::BLACK_PAWN);

    EXPECT_FALSE(board.at(Squares::E4));
    EXPECT_FALSE(board.at(Squares::F3));

    board.undo_move(en_passant);
    print_board(board);

    EXPECT_EQ(board.at(Squares::E4), Pieces::WHITE_PAWN);
    EXPECT_EQ(board.at(Squares::F4), Pieces::BLACK_PAWN);
    EXPECT_FALSE(board.at(Squares::E3));
}

TEST_F(BoardTest, Promotion)
{
    Move setup1(Squares::E2, Squares::E7, NO_TYPE);
    board.make_move(setup1);
    Move black_move(Squares::H7, Squares::H5, NO_TYPE);
    board.make_move(black_move);

    print_board(board);

    Move promote(Squares::E7, Squares::F8, PROMO_QUEEN);
    board.make_move(promote);

    EXPECT_EQ(board.at(Squares::F8), Pieces::WHITE_QUEEN);
    EXPECT_FALSE(board.at(Squares::E7));

    board.undo_move(promote);

    EXPECT_EQ(board.at(Squares::E7), Pieces::WHITE_PAWN);
}