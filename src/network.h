#pragma once

#include <cassert>
#include <fstream>
#include <iostream>

namespace BBD::NNUE
{
// TODO: make HalfKP
class NNUENetwork
{
  public:
    static constexpr int16_t INPUT_SIZE = 768;
    static constexpr int16_t HIDDEN_SIZE = 64;
    static const int16_t evaluation_scale = 400;
    static constexpr int16_t QA = 255;
    static constexpr int16_t QB = 64;
    constexpr static int clipped_relu(int16_t x)
    {
        auto v = std::min(std::max(x, int16_t(0)), QA);
        return v;
    }

  private:
    // simple network
    static std::array<std::array<int16_t, HIDDEN_SIZE>, INPUT_SIZE> weights1;
    static std::array<int16_t, HIDDEN_SIZE> bias1;
    static std::array<std::array<int16_t, HIDDEN_SIZE>, 2> weights2;
    static int16_t bias2;

    // since everything static
    NNUENetwork() = default;

    static inline int16_t swap_bytes(int16_t value)
    {
        return (value << 8) | ((value >> 8) & 0xFF);
    }

  public:
    struct Accumulator
    {
        std::array<int, HIDDEN_SIZE> values{};
        Accumulator()
        {
            refresh();
        };

        void refresh()
        {
            for (int i = 0; i < HIDDEN_SIZE; ++i)
            {
                values[i] = NNUENetwork::bias1[i];
                assert(-32768 <= values[i] && values[i] <= 32768);
            }
        }

        void add_feature(int index)
        {
            for (int i = 0; i < HIDDEN_SIZE; ++i)
            {
                values[i] += NNUENetwork::weights1[index][i];
                assert(-32768 <= values[i] && values[i] <= 32768);
            }
        }

        void remove_feature(int index)
        {
            for (int i = 0; i < HIDDEN_SIZE; ++i)
            {
                values[i] -= NNUENetwork::weights1[index][i];
                assert(-32768 <= values[i] && values[i] <= 32768);
            }
        }
    };

    static int evaluate(const std::array<Accumulator, 2> &acc, bool perspective)
    {
        int32_t output = bias2;

        for (int i = 0; i < HIDDEN_SIZE; ++i)
        {
            // y = o1(p(a)) + o2(p(â)) + c
            output += NNUENetwork::clipped_relu(acc[perspective].values[i]) * weights2[0][i];
            output += NNUENetwork::clipped_relu(acc[1 - perspective].values[i]) * weights2[1][i];
        }

        output *= evaluation_scale;
        output /= (QA * QB);

        return output;
    }

    static bool load_from_file(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file)
            std::cerr << "ERROE!\n";
        if (!file)
            return false;

        file.read(reinterpret_cast<char *>(weights1.data()), sizeof(weights1));
        file.read(reinterpret_cast<char *>(bias1.data()), sizeof(int16_t) * HIDDEN_SIZE);
        file.read(reinterpret_cast<char *>(weights2[0].data()), sizeof(int16_t) * HIDDEN_SIZE);
        file.read(reinterpret_cast<char *>(weights2[1].data()), sizeof(int16_t) * HIDDEN_SIZE);
        file.read(reinterpret_cast<char *>(&bias2), sizeof(int16_t));
        //
        for (int i = 0; i < INPUT_SIZE; ++i)
            for (int j = 0; j < HIDDEN_SIZE; ++j)
                std::cerr << weights1[i][j] << ' ';
        std::cerr << '\n';

        //        std::cerr << "_______ NOW WEIGHTS 2 __________\n";
        //        for (int i = 0; i < HIDDEN_SIZE; ++i)
        //            std::cerr << weights2[0][i] << ' ';
        //        std::cerr << '\n';
        //
        //        for (int i = 0; i < HIDDEN_SIZE; ++i)
        //            std::cerr << weights2[1][i] << ' ';
        //        std::cerr << '\n';
        //        std::cerr << "_________________\n";

        return true;
    }
};
} // namespace BBD::NNUE