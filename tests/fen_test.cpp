#include <gtest/gtest.h>

#include "test_utils.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Tests;

class FENTest : public ::testing::Test
{
  protected:
    Board board;

    void SetUp() override
    {
        board = Board();
    }
};

TEST_F(FENTest, InitialPosition)
{
    Board default_board;
    Board fen_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");

    EXPECT_TRUE(is_equal(default_board, fen_board));
}

TEST_F(FENTest, CustomPosition)
{
    Board fen_board("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 3");

    EXPECT_FALSE(fen_board.at(Squares::B8));
    EXPECT_FALSE(fen_board.at(Squares::E7));

    EXPECT_EQ(fen_board.at(Squares::C6), Pieces::BLACK_KNIGHT);
    EXPECT_EQ(fen_board.at(Squares::E5), Pieces::BLACK_PAWN);
    EXPECT_EQ(fen_board.at(Squares::E4), Pieces::WHITE_PAWN);
    EXPECT_EQ(fen_board.at(Squares::F3), Pieces::WHITE_KNIGHT);

    EXPECT_FALSE(fen_board.at(Squares::E2));
    EXPECT_FALSE(fen_board.at(Squares::G1));

    EXPECT_EQ(fen_board.halfmoves_clock(), 0);
    EXPECT_EQ(fen_board.fullmoves_counter(), 3);

    EXPECT_EQ(fen_board.player_color(), Colors::WHITE);
}