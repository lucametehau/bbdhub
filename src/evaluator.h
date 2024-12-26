#pragma once

#include <iostream>
#include <math.h>

#include "accumulator.h"
#include "board.h"

using namespace BBD;
namespace BBD::NNUE
{
constexpr int LAYERS_AMOUNT = 2;
constexpr int INPUT_SIZE = 768;
constexpr std::array<std::pair<int, int>, LAYERS_AMOUNT> LAYERS_SIZE{
    std::pair<int, int>{INPUT_SIZE, 256},
    std::pair<int, int>{256, 1},
};

template <typename T = float> constexpr T clipped_relu(T x)
{
    if (x < (T)0)
        return (T)0;
    if (x > (T)1)
        return (T)1;
    return x;
}

template <typename T = float> constexpr T *crelu(int size, T *output, const T *input)
{
    for (int i = 0; i < size; ++i)
    {
        output[i] = std::min(std::max(input[i], (T)0), (T)1);
    }

    return output + size;
}
template <typename T> class NNUENetwork;

// TODO: make HalfKP
template <typename T = float> class NNUENetwork
{
  private:
    std::array<LinearLayer<T>, LAYERS_AMOUNT> hidden_weights;

  public:
    // TODO: make loading weights from file
    NNUENetwork()
    {
        for (int i = 0; i < LAYERS_AMOUNT; ++i)
        {
            hidden_weights[i] = LinearLayer<T>(LAYERS_SIZE[i].first, LAYERS_SIZE[i].second);
        }

        // fill random weights
        for (auto &layer : hidden_weights)
            for (int i = 0; i < layer.num_outputs; ++i)
                for (int j = 0; j < layer.num_inputs; ++j)
                    layer.weights[i][j] = (rand() / float(RAND_MAX) - 0.5f);
        // fill random bias
        for (auto &layer : hidden_weights)
            for (int i = 0; i < layer.num_outputs; ++i)
                layer.bias[i] = (rand() / float(RAND_MAX) - 0.5f);
    }

    T evaluate(const NnueAccumulator<T, INPUT_SIZE> &accum, Color perspective)
    {
        // TODO: adequately calc this amount
        T buffer[10 * INPUT_SIZE];
        T input[INPUT_SIZE];
        // load input
        for (int i = 0; i < INPUT_SIZE; ++i)
        {
            input[i] = (accum[perspective])[i];
            //          input[INPUT_SIZE + i] = accum[perspective.flip()][i];
        }

        T *curr_output = buffer;
        T *curr_input = input;

        T *next_output = hidden_weights[0].linear(curr_output, curr_input);
        curr_input = curr_output;
        curr_output = next_output;

        next_output = crelu(hidden_weights[0].num_outputs, curr_output, curr_input);
        curr_input = curr_output;
        curr_output = next_output;

        next_output = hidden_weights[1].linear(curr_output, curr_input);
        return *curr_output;
    }
};

} // namespace BBD::NNUE