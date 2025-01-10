#pragma once

#include <fstream>
#include <iostream>

namespace BBD::NNUE
{
// TODO: make HalfKP
class NNUENetwork
{
  public:
    static constexpr int16_t INPUT_SIZE = 768;
    static constexpr int16_t HIDDEN_SIZE = 1024;
    static const int16_t evaluation_scale = 400;
    static constexpr int16_t QA = 255;
    static constexpr int16_t QB = 64;
    constexpr static int16_t clipped_relu(int16_t x)
    {
        return std::min(std::max(x, int16_t(0)), QA);
    }

  private:
    // simple network
    static std::array<std::array<int16_t, HIDDEN_SIZE>, INPUT_SIZE> weights1;
    static std::array<int16_t, HIDDEN_SIZE> bias1;
    static std::array<int16_t, HIDDEN_SIZE> weights2;
    static int16_t bias2;

    // since everything static
    NNUENetwork() = default;

  public:
    struct Accumulator
    {
        std::array<int16_t, HIDDEN_SIZE> values{};
        Accumulator()
        {
            refresh();
        };

        void refresh()
        {
            for (int i = 0; i < HIDDEN_SIZE; ++i)
                values[i] = NNUENetwork::bias1[i];
        }

        void add_feature(int index)
        {
            for (int i = 0; i < HIDDEN_SIZE; ++i)
            {
                values[i] += NNUENetwork::weights1[index][i];
            }
        }

        void remove_feature(int index)
        {
            for (int i = 0; i < HIDDEN_SIZE; ++i)
            {
                values[i] -= NNUENetwork::weights1[index][i];
            }
        }
    };

    static int evaluate(const Accumulator &acc)
    {
        int32_t output = bias2;

        for (int i = 0; i < HIDDEN_SIZE; ++i)
        {
            output += NNUENetwork::clipped_relu(acc.values[i]) * weights2[i];
        }

        output *= evaluation_scale;
        output /= (QA * QB);

        return output;
    }

    static bool load_from_file(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file)
            return false;

        file.read(reinterpret_cast<char *>(weights1.data()), sizeof(weights1));
        file.read(reinterpret_cast<char *>(bias1.data()), sizeof(bias1));
        file.read(reinterpret_cast<char *>(weights2.data()), sizeof(weights2));
        file.read(reinterpret_cast<char *>(&bias2), sizeof(bias2));

        return true;
    }
};
} // namespace BBD::NNUE