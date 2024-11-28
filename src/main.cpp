#include <iostream>
#include "piece.h"

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
    return 0;
}