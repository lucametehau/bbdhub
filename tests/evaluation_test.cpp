#include <gtest/gtest.h>

#include "eval.h"
#include "test_utils.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Tests;

class EvaluationTest : public ::testing::Test
{
  protected:
    Board board;

    void SetUp() override
    {
        board = Board();
    }
};

TEST_F(EvaluationTest, BasicMoves)
{
    Move pawn_move(Squares::E2, Squares::E4, NO_TYPE);
    board.make_move(pawn_move);

    int score = BBD::Engine::board_evaluation(board);

    EXPECT_EQ(score, 0);

    board.undo_move(pawn_move);

    score = BBD::Engine::board_evaluation(board);

    EXPECT_EQ(score, 0);
}

TEST_F(EvaluationTest, Captures)
{
    Move setup1(Squares::E2, Squares::E4, NO_TYPE);
    Move setup2(Squares::D7, Squares::D5, NO_TYPE);
    board.make_move(setup1);
    board.make_move(setup2);

    Move capture(Squares::E4, Squares::D5, NO_TYPE);
    board.make_move(capture);

    int score = BBD::Engine::board_evaluation(board);

    EXPECT_EQ(score, 100);

    board.undo_move(capture);

    score = BBD::Engine::board_evaluation(board);

    EXPECT_EQ(score, 0);
}
