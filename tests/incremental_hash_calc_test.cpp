#include <gtest/gtest.h>

#include "test_utils.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Tests;

// -----------------------------------------------------------------------------
// Test fixture for Hash Calculation
// -----------------------------------------------------------------------------
class IncrementalHashCalcTest : public ::testing::Test
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


TEST_F(IncrementalHashCalcTest, BasicMoves)
{
    uint64_t hash1 = board.hash_calc();
    uint64_t hash1_ = board.get_cur_hash();
    EXPECT_EQ(hash1, hash1_);
    Move pawn_move(Squares::E2, Squares::E4, NO_TYPE);
    board.make_move(pawn_move);
    uint64_t hash2 = board.hash_calc();
    uint64_t hash2_ = board.get_cur_hash();
    EXPECT_EQ(hash2, hash2_);
    EXPECT_FALSE(hash1 == hash2);

    board.undo_move(pawn_move);

    uint64_t hash3 = board.hash_calc();
    uint64_t hash3_ = board.get_cur_hash();
    EXPECT_EQ(hash3, hash3_);

    EXPECT_EQ(hash1, hash3);
}

TEST_F(IncrementalHashCalcTest, BasicMoves2)
{
    uint64_t hash0 = board.hash_calc();
    uint64_t hash0_ = board.get_cur_hash();
    EXPECT_EQ(hash0, hash0_);

    // Move pawn_move1(Squares::E2, Squares::E4, NO_TYPE);
    // Move pawn_move2(Squares::B7, Squares::B5, NO_TYPE);
    Move knight_move1(Squares::G8, Squares::F6, NO_TYPE);
    Move knight_move2(Squares::B1, Squares::C3, NO_TYPE);

    board.make_move(knight_move1);
    board.make_move(knight_move2);

    uint64_t hash1 = board.hash_calc();
    uint64_t hash1_ = board.get_cur_hash();
    EXPECT_EQ(hash1, hash1_);

    board.undo_move(knight_move2);
    board.undo_move(knight_move1);

    uint64_t hash01 = board.hash_calc();
    uint64_t hash01_ = board.get_cur_hash();
    EXPECT_EQ(hash01, hash01_);

    EXPECT_EQ(hash01, hash0);

    board.make_move(knight_move2);
    board.make_move(knight_move1);

    uint64_t hash2 = board.hash_calc();
    uint64_t hash2_ = board.get_cur_hash();
    EXPECT_EQ(hash2, hash2_);

    EXPECT_EQ(hash1, hash2);
}

TEST_F(IncrementalHashCalcTest, Captures)
{
    Move setup1(Squares::E2, Squares::E4, NO_TYPE);
    Move setup2(Squares::D7, Squares::D5, NO_TYPE);
    board.make_move(setup1);
    board.make_move(setup2);

    uint64_t hash1 = board.hash_calc();
    uint64_t hash1_ = board.get_cur_hash();
    EXPECT_EQ(hash1, hash1_);

    Move capture(Squares::E4, Squares::D5, NO_TYPE);
    board.make_move(capture);

    uint64_t hash2 = board.hash_calc();
    uint64_t hash2_ = board.get_cur_hash();
    EXPECT_EQ(hash2, hash2_);

    EXPECT_FALSE(hash1 == hash2);

    board.undo_move(capture);

    uint64_t hash3 = board.hash_calc();
    uint64_t hash3_ = board.get_cur_hash();
    EXPECT_EQ(hash3, hash3_);

    EXPECT_EQ(hash1, hash3);
}

TEST_F(IncrementalHashCalcTest, Captures2)
{
    Move setup1(Squares::B8, Squares::C3, NO_TYPE);
    Move setup2(Squares::G1, Squares::E4, NO_TYPE);
    board.make_move(setup1);
    board.make_move(setup2);

    Move capture1(Squares::B1, Squares::C3, NO_TYPE);
    Move capture2(Squares::E4, Squares::C3, NO_TYPE);

    board.make_move(capture1);
    Move setup3(Squares::C3, Squares::B1, NO_TYPE);
    board.make_move(setup3);
    uint64_t hash1 = board.hash_calc();
    uint64_t hash1_ = board.get_cur_hash();
    EXPECT_EQ(hash1, hash1_);

    board.undo_move(setup3);
    board.undo_move(capture1);

    board.make_move(capture2);
    Move setup4(Squares::C3, Squares::E4, NO_TYPE);
    board.make_move(setup4);
    uint64_t hash2 = board.hash_calc();
    uint64_t hash2_ = board.get_cur_hash();
    EXPECT_EQ(hash2, hash2_);

    EXPECT_EQ(hash1, hash2);
}

TEST_F(IncrementalHashCalcTest, Castling)
{
    Move clear1(Squares::E2, Squares::E4, NO_TYPE);
    Move clear2(Squares::G1, Squares::F3, NO_TYPE);
    Move clear3(Squares::F1, Squares::E2, NO_TYPE);
    Move clear4(Squares::D7, Squares::D5, NO_TYPE);

    board.make_move(clear1);
    board.make_move(clear2);
    board.make_move(clear3);
    board.make_move(clear4);

    uint64_t hash1 = board.hash_calc();
    uint64_t hash1_ = board.get_cur_hash();
    EXPECT_EQ(hash1, hash1_);
    Move castle(Squares::E1, Squares::G1, CASTLE);
    board.make_move(castle);

    uint64_t hash2 = board.hash_calc();
    uint64_t hash2_ = board.get_cur_hash();
    EXPECT_EQ(hash2, hash2_);

    EXPECT_FALSE(hash1 == hash2);

    board.undo_move(castle);

    uint64_t hash3 = board.hash_calc();
    uint64_t hash3_ = board.get_cur_hash();
    EXPECT_EQ(hash3, hash3_);

    EXPECT_EQ(hash1, hash3);
}

TEST_F(IncrementalHashCalcTest, WhiteEnPassant)
{
    Move clear1(Squares::E2, Squares::E5, NO_TYPE);
    Move clear2(Squares::F7, Squares::F5, NO_TYPE);
    Move en_passant(Squares::E5, Squares::F6, ENPASSANT);
    board.make_move(clear1);
    board.make_move(clear2);

    uint64_t hash1 = board.hash_calc();
    uint64_t hash1_ = board.get_cur_hash();
    EXPECT_EQ(hash1, hash1_);

    board.make_move(en_passant);

    uint64_t hash2 = board.hash_calc();
    uint64_t hash2_ = board.get_cur_hash();
    EXPECT_EQ(hash2, hash2_);

    EXPECT_FALSE(hash1 == hash2);

    board.undo_move(en_passant);

    uint64_t hash3 = board.hash_calc();
    uint64_t hash3_ = board.get_cur_hash();
    EXPECT_EQ(hash3, hash3_);

    EXPECT_EQ(hash1, hash3);
}

TEST_F(IncrementalHashCalcTest, BlackEnPassant)
{
    Move clear1(Squares::G2, Squares::G4, NO_TYPE);
    Move clear2(Squares::F7, Squares::F4, NO_TYPE);
    Move clear3(Squares::E2, Squares::E4, NO_TYPE);
    Move en_passant(Squares::F4, Squares::E3, ENPASSANT);
    board.make_move(clear1);
    board.make_move(clear2);
    board.make_move(clear3);

    uint64_t hash1 = board.hash_calc();
    uint64_t hash1_ = board.get_cur_hash();
    EXPECT_EQ(hash1, hash1_);

    board.make_move(en_passant);

    uint64_t hash2 = board.hash_calc();
    uint64_t hash2_ = board.get_cur_hash();
    EXPECT_EQ(hash2, hash2_);

    EXPECT_FALSE(hash1 == hash2);

    board.undo_move(en_passant);

    uint64_t hash3 = board.hash_calc();
    uint64_t hash3_ = board.get_cur_hash();
    EXPECT_EQ(hash3, hash3_);

    EXPECT_EQ(hash1, hash3);
}


TEST_F(IncrementalHashCalcTest, Promo1)
{
    Move setup1(Squares::B2, Squares::C3, NO_TYPE);
    Move setup2(Squares::B1, Squares::A3, NO_TYPE);
    Move setup3(Squares::B7, Squares::B2, NO_TYPE);
    board.make_move(setup1);
    board.make_move(setup2);
    board.make_move(setup3);

    uint64_t hash1 = board.hash_calc();
    uint64_t hash1_ = board.get_cur_hash();
    EXPECT_EQ(hash1, hash1_);

    Move promo1(Squares::B2, Squares::B1, PROMO_QUEEN);
    board.make_move(promo1);

    uint64_t hash2 = board.hash_calc();
    uint64_t hash2_ = board.get_cur_hash();
    EXPECT_EQ(hash2, hash2_);
}

TEST_F(IncrementalHashCalcTest, Promo2)
{
    Move setup0(Squares::A2, Squares::A3, NO_TYPE);
    board.make_move(setup0);

    Move setup1(Squares::B2, Squares::C3, NO_TYPE);
    Move setup2(Squares::B1, Squares::A3, NO_TYPE);
    Move setup3(Squares::B7, Squares::B2, NO_TYPE);
    board.make_move(setup1);
    board.make_move(setup2);
    board.make_move(setup3);

    uint64_t hash1 = board.hash_calc();
    uint64_t hash1_ = board.get_cur_hash();
    EXPECT_EQ(hash1, hash1_);

    Move promo1(Squares::B2, Squares::B1, PROMO_QUEEN);
    board.make_move(promo1);

    uint64_t hash2 = board.hash_calc();
    uint64_t hash2_ = board.get_cur_hash();
    EXPECT_EQ(hash2, hash2_);
}