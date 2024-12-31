#include <gtest/gtest.h>

#include "test_utils.h"
#include <iostream>
#include <math.h>

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
TEST_F(NNUETest, IO_Test)
{
    // random weights now
    network.write_to_file("test.bin");
    auto nn1 = "nn1.txt";
    std::ofstream file(nn1);
    for (auto &layer : network.get_layers())
    {
        layer.cerr_print(file);
    }
    file.close();
    NNUENetwork<float> new_network;
    new_network.load_from_file("test.bin");
    auto nn2 = "nn2.txt";
    file = std::ofstream(nn2);
    for (auto &layer : new_network.get_layers())
    {
        layer.cerr_print(file);
    }
    file.close();

    EXPECT_TRUE(is_equal_networks(network, new_network));
}
TEST_F(NNUETest, Learn_Test)
{
    board = Board("2bq1rk1/pr3ppn/1p2p3/7P/2pP1B1P/2P5/PPQ2PB1/R3R1K1 w - -");
    float input[INPUT_SIZE];
    auto accum = board.get_accumulator();
    for (int i = 0; i < INPUT_SIZE; ++i)
    {
        input[i] = accum[Colors::WHITE][i];
    }

    float scaling = 410.f;
    float raw_target = 311.0f / scaling;

    auto initial_eval = network.forward(input);
    float initial_error = std::abs(initial_eval - raw_target);

    for (int i = 0; i < 10; i++)
    {
        std::cerr << "train epoch: " << i << " ... ";
        network.train(accum, Colors::WHITE, raw_target);
        std::cerr << "finished\n";
    }

    // Check final error
    float final_eval = network.forward(input);
    float final_error = std::abs(final_eval - raw_target);

    EXPECT_LE(final_error, initial_error)
        << "Initial eval: " << initial_eval << " Final eval: " << final_eval << " Target: " << raw_target;
}