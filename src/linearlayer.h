#pragma once
#include <iostream>
#include <vector>

template <typename T = float> class LinearLayer
{
  public:
    int num_inputs;
    int num_outputs;
    T **weights;
    T *bias;

    constexpr LinearLayer() = default;

    constexpr LinearLayer(int num_inputs, int num_outputs)
        : num_inputs(num_inputs), num_outputs(num_outputs), weights(new T *[num_inputs]), bias(new T[num_outputs])
    {
        for (int i = 0; i < num_inputs; ++i)
            weights[i] = new T[num_outputs];
    }

    T *linear(T *output, const T *input)
    {
        for (int i = 0; i < num_outputs; ++i)
            output[i] = bias[i];
        for (int i = 0; i < num_inputs; ++i)
        {
            for (int j = 0; j < num_outputs; ++j)
            {
                output[j] += input[i] * weights[i][j];
            }
        }

        return output + num_outputs;
    }

    constexpr int get_input_size() const
    {
        return num_inputs;
    }
    constexpr int get_output_size() const
    {
        return num_outputs;
    }
};

// typedef LinearLayer<> LinearLayer;