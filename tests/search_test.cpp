#include <gtest/gtest.h>

#include "test_utils.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Tests;
using namespace BBD::Engine;

class SearchTest : public ::testing::Test
{
  protected:
    Board board;

    void SetUp() override
    {
        board = Board();
    }
};

TEST_F(SearchTest, TimeSearch1sec)
{
    BBD::Engine::init("../../drill/nnue_v1-100/quantised.bin");
    Board board;
    SearchLimiter limiter;
    limiter.set_time(1000);
    SearchThread thread;

    print_board(board);
    auto time_before = get_time_since_start();

    thread.search(board, limiter);
    auto t = (get_time_since_start() - time_before) / 1000.0; // in seconds
    std::cerr << t;
    EXPECT_LE(t, 1.005);
}
TEST_F(SearchTest, TimeSearch5sec)
{
    BBD::Engine::init();
    Board board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    SearchLimiter limiter;
    limiter.set_time(5000);
    SearchThread thread;

    print_board(board);
    auto time_before = get_time_since_start();

    thread.search(board, limiter);
    auto t = (get_time_since_start() - time_before) / 1000.0; // in seconds
    std::cerr << t;
    EXPECT_LE(t, 5);
}