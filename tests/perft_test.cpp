#include <gtest/gtest.h>

#include "test_utils.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Tests;

class PerftTest : public ::testing::Test
{
  protected:
    Board board;

    void SetUp() override
    {
        board = Board();
    }
};

// TEST_F(PerftTest, StartPos)
//{
//     BBD::attacks::init();
//     Board board;
//
//     print_board(board);
//
//     EXPECT_EQ(perft(board, 1, true), 20);
//
//     EXPECT_EQ(perft(board, 4, true), 197281);
//     EXPECT_EQ(perft(board, 5, true), 4865609);
// }

// TEST_F(PerftTest, Kiwipete)
//{
//     BBD::attacks::init();
//     Board board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
//
//     print_board(board);
//
//     EXPECT_EQ(perft(board, 1, true), 48);
//
//     EXPECT_EQ(perft(board, 4, true), 4085603);
//     // EXPECT_EQ(perft(board, 5, true), 193690690);
// }

// TEST_F(PerftTest, EnpassantPos)
//{
//     BBD::attacks::init();
//     Board board("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
//
//     print_board(board);
//
//     EXPECT_EQ(perft(board, 1, true), 14);
//
//     EXPECT_EQ(perft(board, 4, true), 43238);
//     EXPECT_EQ(perft(board, 5, true), 674624);
//     EXPECT_EQ(perft(board, 6, true), 11030083);
// }

// TEST_F(PerftTest, CustomPos4)
//{
//     BBD::attacks::init();
//     Board board("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
//
//     print_board(board);
//
//     EXPECT_EQ(perft(board, 1, true), 6);
//
//     EXPECT_EQ(perft(board, 4, true), 422333);
//     EXPECT_EQ(perft(board, 5, true), 15833292);
// }

// TEST_F(PerftTest, CustomPos5)
//{
//     BBD::attacks::init();
//     Board board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
//
//     print_board(board);
//
//     EXPECT_EQ(perft(board, 1, true), 44);
//
//     EXPECT_EQ(perft(board, 4, true), 2103487);
//     // EXPECT_EQ(perft(board, 5, true), 89941194);
// }
