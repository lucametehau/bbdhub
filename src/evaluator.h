#pragma once

#include <iostream>

#include "accumulator.h"
#include "board.h"

using namespace BBD;
namespace BBD::NNUE
{
constexpr int LAYERS_AMOUNT = 2;
constexpr int INPUT_SIZE = 8;
constexpr std::array<std::pair<int, int>, LAYERS_AMOUNT> LAYERS_SIZES{
    std::pair<int, int>{INPUT_SIZE, 8},
    std::pair<int, int>{8, 1},
};

template <typename T = float> constexpr T clipped_relu(T x)
{
    if (x < (T)0)
        return (T)0;
    if (x > (T)1)
        return (T)1;
    return x;
}
template <typename T = float> constexpr T clipped_relu_der(T x)
{
    if (x < (T)0 || x > (T)1)
        return (T)0;
    return (T)1;
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
    typedef TrainLayer<T> Layer; // for training
                                 //    typedef LinearLayer<T> Layer; // for evaluating
    std::array<Layer, LAYERS_AMOUNT> hidden_weights;
    //    {
    //        Layer(LAYERS_SIZES[0].first, LAYERS_SIZES[0].second),
    //        Layer(LAYERS_SIZES[1].first, LAYERS_SIZES[1].second),
    //    };
    float learning_rate = 0.01;

  public:
    NNUENetwork()
    {
        for (int i = 0; i < LAYERS_AMOUNT; ++i)
        {
            hidden_weights[i] = Layer(LAYERS_SIZES[i].first, LAYERS_SIZES[i].second);
        }
        // fill random weights
        for (auto &layer : hidden_weights)
        {
            for (int i = 0; i < layer.num_inputs; ++i)
                for (int j = 0; j < layer.num_outputs; ++j)
                {
                    layer.weights[i][j] = (T)(rand() / float(RAND_MAX) - 0.5f);
                }
        }
        // fill random bias
        for (auto &layer : hidden_weights)
            for (int i = 0; i < layer.num_outputs; ++i)
                layer.bias[i] = (T)(rand() / float(RAND_MAX) - 0.5f);
    }

    void train(const NnueAccumulator<T, INPUT_SIZE> &accum, Color perspective, T expected)
    {
        T input[INPUT_SIZE];
        for (int i = 0; i < INPUT_SIZE; ++i)
        {
            input[i] = accum[perspective][i];
        }
        forward(input);
        backward_propagate(expected, input);
    }
    T forward(T *input)
    {
        //      assert(typeid(Layer) == TrainLayer<T>);
        hidden_weights[0].linear(hidden_weights[1].raw, input);
        hidden_weights[0].activate(clipped_relu);
        T *result = new T[1];
        hidden_weights[1].linear(result, hidden_weights[0].activated);
        return *result;
    }
    void backward_propagate(T expected, T *input)
    {
        T delta = *(hidden_weights.back().activated) - expected;
        T error = delta; // squared?

        // compute gradients
        std::vector<T> gradients(LAYERS_SIZES.back().first);
        for (int i = 0; i < hidden_weights.back().num_inputs; ++i)
        {
            T gradient_weight = error * hidden_weights[LAYERS_AMOUNT - 2].activated[i];
            hidden_weights.back().weights[i][0] -= learning_rate * gradient_weight;

            gradients[i] = error * hidden_weights.back().weights[i][0];
        }

        // tweak
        hidden_weights.back().bias[0] -= error * learning_rate;

        for (int layer_ind = LAYERS_AMOUNT - 2; layer_ind >= 0; --layer_ind)
        {
            auto &layer = hidden_weights[layer_ind];
            std::vector<T> next_gradients(layer.num_inputs);
            for (int i = 0; i < layer.num_inputs; ++i)
            {
                next_gradients[i] = 0;
                for (int j = 0; j < layer.num_outputs; ++j)
                {
                    T gradient = gradients[j] * clipped_relu_der(layer.raw[j]);

                    T gradient_weight = gradient * input[i];
                    if (layer_ind != 0)
                        gradient_weight = gradient * hidden_weights[layer_ind - 1].activated[i];

                    layer.weights[i][j] -= learning_rate * gradient_weight;
                    next_gradients[i] += gradient * layer.weights[i][j];
                }
            }

            for (int j = 0; j < layer.num_inputs; ++j)
            {
                T gradient = gradients[j] * clipped_relu_der(layer.raw[j]);
                layer.bias[j] -= learning_rate * gradient;
            }
            // recalc gradients
            gradients = std::move(next_gradients);
        }
    }
    T evaluate(const NnueAccumulator<T, INPUT_SIZE> &accum, Color perspective)
    {
        // TODO: think about if allocating memory takes too much time
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

        // rewrite into a loop
        T *next_output = hidden_weights[0].linear(curr_output, curr_input);
        curr_input = curr_output;
        curr_output = next_output;
        next_output = crelu(hidden_weights[0].num_outputs, curr_output, curr_input);
        curr_input = curr_output;
        curr_output = next_output;
        next_output = hidden_weights[1].linear(curr_output, curr_input);
        return *curr_output;
    }

    bool write_to_file(const std::string &filename) const
    {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open())
            return false;

        try
        {
            file.write(reinterpret_cast<const char *>(&LAYERS_AMOUNT), sizeof(LAYERS_AMOUNT));

            for (const auto &layer : hidden_weights)
            {
                layer.write(file);
            }
        }
        catch (...)
        {
            std::cerr << "Error while writing network weights!\n";
            return false;
        }
        return true;
    }
    bool load_from_file(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open())
            return false;

        try
        {
            int stored_layers;
            file.read(reinterpret_cast<char *>(&stored_layers), sizeof(stored_layers));
            if (stored_layers != LAYERS_AMOUNT)
                return false;

            for (auto &layer : hidden_weights)
            {
                if (!layer.read(file))
                    return false;
            }
        }
        catch (...)
        {
            std::cerr << "Error while reading network weights!\n";
            return false;
        }

        return true;
    }

    auto get_layers() const
    {
        return hidden_weights;
    }
};
} // namespace BBD::NNUE