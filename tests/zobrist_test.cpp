#include <gtest/gtest.h>

#include "test_utils.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Tests;

class ZobristTest : public ::testing::Test
{
  protected:
    Board board;

    void SetUp() override
    {
        board = Board();
    }
};

TEST_F(ZobristTest, BasicMoves)
{
    Move pawn_move(Squares::E2, Squares::E4, NO_TYPE);
    board.make_move(pawn_move);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());

    board.undo_move(pawn_move);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());

}

TEST_F(ZobristTest, BasicMoves2)
{
    Move pawn_move(Squares::F2, Squares::F4, NO_TYPE);
    board.make_move(pawn_move);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());

    board.undo_move(pawn_move);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());

}

TEST_F(ZobristTest, Captures)
{
    Move setup1(Squares::E2, Squares::E4, NO_TYPE);
    Move setup2(Squares::D7, Squares::D5, NO_TYPE);
    board.make_move(setup1);
    board.make_move(setup2);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());

    Move capture(Squares::E4, Squares::D5, NO_TYPE);
    board.make_move(capture);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());

    board.undo_move(capture);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());
}

TEST_F(ZobristTest, Castling)
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

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());

    board.undo_move(castle);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());
}

TEST_F(ZobristTest, WhiteEnPassant)
{
    Move clear1(Squares::E2, Squares::E5, NO_TYPE);
    Move clear2(Squares::F7, Squares::F5, NO_TYPE);
    Move en_passant(Squares::E5, Squares::F6, ENPASSANT);
    board.make_move(clear1);
    board.make_move(clear2);

    board.make_move(en_passant);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());

    board.undo_move(en_passant);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());
}

TEST_F(ZobristTest, BlackEnPassant)
{
    Move clear1(Squares::G2, Squares::G4, NO_TYPE);
    Move clear2(Squares::F7, Squares::F4, NO_TYPE);
    Move clear3(Squares::E2, Squares::E4, NO_TYPE);
    Move en_passant(Squares::F4, Squares::E3, ENPASSANT);
    board.make_move(clear1);
    board.make_move(clear2);
    board.make_move(clear3);

    board.make_move(en_passant);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());

    board.undo_move(en_passant);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());
}

TEST_F(ZobristTest, Promotion)
{
    Move setup1(Squares::E2, Squares::E7, NO_TYPE);
    board.make_move(setup1);
    Move black_move(Squares::H7, Squares::H5, NO_TYPE);
    board.make_move(black_move);

    Move promote(Squares::E7, Squares::F8, PROMO_QUEEN);
    board.make_move(promote);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());

    board.undo_move(promote);

    EXPECT_EQ(BBD::Zobrist::hash_calc(board), board.get_current_zobrist_hash());
}
