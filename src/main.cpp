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
    return 0;
}