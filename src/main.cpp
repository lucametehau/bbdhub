#include "attacks.h"
#include "board.h"
#include "move.h"
#include "piece.h"
#include "square.h"
#include <iostream>

using namespace BBD; // DON'T LEAVE THIS HERE

int main(int argc, char *argv[])
{
    BBD::attacks::init();
    // Decomment this when parser is implemented!
    /*
    if (argc != 1) {
        std::cerr << "Expected 5 arguments!\nYour command should look like ./BBD
    -H <input history file> -m <output move file>."; return 0;
    }
    */

    using namespace BBD::attacks;
    using namespace BBD::Squares;
    king_attacks[E2].print();
    knight_attacks[E4].print();
    Bitboard occ = (1ull << E3) | (1ull << D5);
    generate_attacks<PieceTypes::BISHOP>(G2, occ).print();
    generate_attacks<PieceTypes::BISHOP>(E4, occ).print();
    generate_attacks<PieceTypes::ROOK>(E4, occ).print();
    generate_attacks<PieceTypes::ROOK>(D1, occ).print();

    Board board;

    Move move(D8, E6, MoveTypes::NO_TYPE);
    std::cout << move.to_string() << "\n"; // d8e6
    Move move2(Squares::E2, Squares::E4, MoveTypes::NO_TYPE);
    std::cout << move2.to_string() << "\n"; // e2e4
    move = Move(E2, E1, MoveTypes::PROMO_ROOK);
    std::cout << move.to_string() << "\n"; // e2e1q
    return 0;
}