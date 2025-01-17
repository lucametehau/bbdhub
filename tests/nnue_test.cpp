#include <fstream>
#include <gtest/gtest.h>
#include <random>

#include "board.h"
#include "network.cpp"
using namespace BBD;
using namespace BBD::NNUE;

class NNUETest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        create_test_network();
    }

    void TearDown() override
    {
        std::remove("test.bin");
    }

    void create_test_network()
    {
        std::ofstream file("test.bin", std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(file.is_open());

        std::mt19937 rng(42);
        std::uniform_int_distribution<int16_t> dist(-1024, 1024);

        std::array<std::array<int16_t, NNUENetwork::HIDDEN_SIZE>, NNUENetwork::INPUT_SIZE> weights1{};
        std::array<int16_t, NNUENetwork::HIDDEN_SIZE> bias1{};
        std::array<int16_t, NNUENetwork::HIDDEN_SIZE> weights2{};
        int16_t bias2;

        for (auto &row : weights1)
            for (auto &w : row)
                w = dist(rng);

        for (auto &v : bias1)
            v = dist(rng);

        for (auto &w : weights2)
            w = dist(rng);

        bias2 = dist(rng);

        file.write(reinterpret_cast<char *>(weights1.data()), sizeof(weights1));
        file.write(reinterpret_cast<char *>(bias1.data()), sizeof(bias1));
        file.write(reinterpret_cast<char *>(weights2.data()), sizeof(weights2));
        file.write(reinterpret_cast<char *>(&bias2), sizeof(bias2));

        // Add padding to 64-byte boundary
        size_t data_size = sizeof(weights1) + sizeof(bias1) + sizeof(weights2) + sizeof(bias2);
        size_t padding_size = (64 - (data_size % 64)) % 64;
        std::vector<char> padding(padding_size, 0);
        if (padding_size > 0)
        {
            file.write(padding.data(), padding_size);
        }
    }
};

TEST_F(NNUETest, NetworkLoading)
{
    EXPECT_TRUE(NNUENetwork::load_from_file("test.bin"));
    EXPECT_FALSE(NNUENetwork::load_from_file("somthing"));
}

TEST_F(NNUETest, InitialPosition)
{
    ASSERT_TRUE(NNUENetwork::load_from_file("test.bin"));

    Board board;
    int eval = NNUENetwork::evaluate(board.get_accumulators(), Colors::WHITE);
    std::cerr << eval << std::endl;
}

TEST_F(NNUETest, IncrementalSimpleMove)
{
    using namespace Squares;

    Board board;
    int eval_start = NNUENetwork::evaluate(board.get_accumulators(), Colors::WHITE);
    board.make_move(Move(E2, E4, NO_TYPE));
    int eval1 = NNUENetwork::evaluate(board.get_accumulators(), Colors::BLACK);

    board.refresh_accumulators();
    int eval2 = NNUENetwork::evaluate(board.get_accumulators(), Colors::BLACK);

    ASSERT_EQ(eval1, eval2);

    board.undo_move(Move(E2, E4, NO_TYPE));
    ASSERT_EQ(NNUENetwork::evaluate(board.get_accumulators(), Colors::WHITE), eval_start);
}

TEST_F(NNUETest, EnpassantTest)
{
    using namespace Squares;

    Board board;
    board.set_fen("r1bqkbnr/1ppp2pp/2n5/1B2ppP1/p3P3/5N2/PPPP1P1P/RNBQK2R w KQkq f6 0 6");
    int eval_start = NNUENetwork::evaluate(board.get_accumulators(), Colors::WHITE);
    board.make_move(Move(G5, F6, ENPASSANT));
    int eval1 = NNUENetwork::evaluate(board.get_accumulators(), Colors::BLACK);

    board.refresh_accumulators();
    int eval2 = NNUENetwork::evaluate(board.get_accumulators(), Colors::BLACK);

    ASSERT_EQ(eval1, eval2);

    board.undo_move(Move(G5, F6, ENPASSANT));
    ASSERT_EQ(NNUENetwork::evaluate(board.get_accumulators(), Colors::WHITE), eval_start);
}