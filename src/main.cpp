#include <iostream>
#include "piece.h"
#include "square.h"
#include "move.h"
#include "attacks.h"

using namespace BBD; // DON'T LEAVE THIS HERE

int main(int argc, char* argv[]) {
    BBD::attacks::init();
    // Decomment this when parser is implemented!
    /*
    if (argc != 1) {
        std::cerr << "Expected 5 arguments!\nYour command should look like ./BBD -H <input history file> -m <output move file>.";
        return 0;
    }
    */

    using namespace BBD::Squares;

    Move move(D8, E6, MoveTypes::NO_TYPE);
    std::cout << move.to_string() << "\n"; // d8e6
    move = Move(E2, E1, MoveTypes::PROMO_ROOK);
    std::cout << move.to_string() << "\n"; // e2e1q

    using namespace BBD::attacks;
    print_mask(king_attacks[E1]);
    print_mask(knight_attacks[E4]);
    return 0;
}