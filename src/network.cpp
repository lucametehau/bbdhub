#include "network.h"

namespace BBD::NNUE
{

std::array<std::array<int16_t, NNUENetwork::HIDDEN_SIZE>, NNUENetwork::INPUT_SIZE> NNUENetwork::weights1;
std::array<int16_t, NNUENetwork::HIDDEN_SIZE> NNUENetwork::bias1;
std::array<std::array<int16_t, NNUENetwork::HIDDEN_SIZE>, 2> NNUENetwork::weights2;
int16_t NNUENetwork::bias2;
} // namespace BBD::NNUE