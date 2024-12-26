#include <gtest/gtest.h>

#include "test_utils.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Tests;
using namespace BBD::Engine;
using namespace BBD::NNUE;

class NNUETest : public ::testing::Test
{
  protected:
    Board board;
    NNUENetwork<float> network;

    void SetUp() override
    {
        board = Board();
        network = NNUENetwork<float>();
    }
};

TEST_F(NNUETest, InitialPos)
{
    std::cout << network.evaluate(board.get_accumulator(), Colors::WHITE);
}