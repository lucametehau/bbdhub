#include <gtest/gtest.h>

#include "test_utils.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Tests;

class ThreeFoldTest : public ::testing::Test
{
  protected:
    Board board;

    void SetUp() override
    {
        BBD::Zobrist::init();
        board = Board();
    }
};