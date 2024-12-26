#pragma once

#include <vector>

#include "color.h"
#include "linearlayer.h"

using namespace BBD;
namespace BBD::NNUE
{

// TODO: make template
template <typename T, int M> struct NnueAccumulator
{
    // Two vectors of size M. v[0] for white's, and v[1] for black's perspectives.
    T v[2][M];

    const T *operator[](Color perspective) const
    {
        return v[perspective];
    }

    T *operator[](Color perspective)
    {
        return v[perspective];
    }

    void fill(T value)
    {
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < M; ++j)
                v[i][j] = value;
    }

    void refresh(const std::array<float, M> &bias, Color perspective)
    {
        for (int i = 0; i < M; ++i)
            v[perspective][i] = bias[i];
    }
    // TODO: check this later
    void refresh(const LinearLayer<T> &layer, const std::vector<int> &active_features, Color perspective)
    {
        for (int i = 0; i < M; ++i)
            v[perspective][i] = layer.bias[i];

        for (int a : active_features)
        {
            for (int i = 0; i < M; ++i)
            {
                v[perspective][i] += layer.weights[a][i];
            }
        }
    }

    // TODO: finish after updating accumulator incrementally
    void update_accumulator(const LinearLayer<T> &layer, const NnueAccumulator<T, M> &prev_accumulator,
                            NnueAccumulator<T, M> &new_accumulator, const std::vector<int> &removed_features,
                            const std::vector<int> &added_features, Color perspective);
};
} // namespace BBD::NNUE
// The inspiration for most of the code
// was taken here: https://github.com/official-stockfish/nnue-pytorch/blob/master/docs/nnue.md#linear-layer