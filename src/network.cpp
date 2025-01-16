#include "network.h"

namespace BBD::NNUE
{

#ifdef EVALFILE
INCBIN(NetworkData, EVALFILE);
#endif

std::array<std::array<int16_t, NNUENetwork::HIDDEN_SIZE>, NNUENetwork::INPUT_SIZE> NNUENetwork::weights1;
std::array<int16_t, NNUENetwork::HIDDEN_SIZE> NNUENetwork::bias1;
std::array<std::array<int16_t, NNUENetwork::HIDDEN_SIZE>, 2> NNUENetwork::weights2;
int16_t NNUENetwork::bias2;

bool NNUENetwork::load_from_file(const std::string &filename)
{
#ifndef EVALFILE
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
#else
    std::size_t idx = 0;
    std::memcpy(weights1.data(), &gNetworkDataData[idx], sizeof(weights1));
    idx += sizeof(weights1);
    std::memcpy(bias1.data(), &gNetworkDataData[idx], sizeof(bias1));
    idx += sizeof(bias1);
    std::memcpy(weights2[0].data(), &gNetworkDataData[idx], sizeof(weights2[0]));
    idx += sizeof(weights2[0]);
    std::memcpy(weights2[1].data(), &gNetworkDataData[idx], sizeof(weights2[1]));
    idx += sizeof(weights2[1]);
    std::memcpy(&bias2, &gNetworkDataData[idx], sizeof(int16_t));
    idx += sizeof(int16_t);
#endif

    return true;
}
} // namespace BBD::NNUE