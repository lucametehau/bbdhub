#include <iostream>
#include "piece.h"
#include "square.h"


int main(int argc, char* argv[]) {
    // Decomment this when parser is implemented!
    /*
    if (argc != 1) {
        std::cerr << "Expected 5 arguments!\nYour command should look like ./BBD -H <input history file> -m <output move file>.";
        return 0;
    }
    */

    std::cout << BBD::Pieces::WHITE_PAWN.to_char() << "\n";
    std::cout << BBD::Pieces::WHITE_BISHOP.to_char() << "\n";
    std::cout << BBD::Pieces::BLACK_KNIGHT.to_char() << "\n";
    std::cout << BBD::Pieces::BLACK_QUEEN.to_char() << "\n";
    std::cout << BBD::Pieces::WHITE_KING.to_char() << "\n";

    std::cout << BBD::Squares::A6 << "\n"; // 40
    std::cout << BBD::Squares::B7 << "\n"; // 49
    std::cout << BBD::Squares::C8 << "\n"; // 58

    return 0;
}