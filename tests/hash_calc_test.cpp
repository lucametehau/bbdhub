#include <gtest/gtest.h>

#include "test_utils.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Tests;

// -----------------------------------------------------------------------------
// Test fixture for Hash Calculation
// -----------------------------------------------------------------------------
class HashCalcTest : public ::testing::Test
{
  protected:
    Board board;

    void SetUp() override
    {
        BBD::Zobrist::init();
        board = Board();
    }
};

// -----------------------------------------------------------------------------
// 1) BasicMoves
// -----------------------------------------------------------------------------
