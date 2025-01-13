#include <gtest/gtest.h>

#include "test_utils.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Tests;

class ThreeFoldTest : public ::testing::Test
{
  protected:
    Board board;

    void SetUp() override
    {
        BBD::Zobrist::init();
        board = Board();
    }
};


TEST_F(ThreeFoldTest, BasicMoves)
{
    Move move1(Squares::B1, Squares::C3, NO_TYPE);
    Move move2(Squares::C3, Squares::B1, NO_TYPE);
    EXPECT_FALSE(board.threefold_check());
    board.make_move(move1);
    EXPECT_FALSE(board.threefold_check());
    board.make_move(move2);
    EXPECT_FALSE(board.threefold_check());
    board.make_move(move1);
    EXPECT_FALSE(board.threefold_check());
    board.make_move(move2);
    EXPECT_TRUE(board.threefold_check());
}