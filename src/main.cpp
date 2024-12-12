#include "../tests/test_utils.h"
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
    Board board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    std::cout << Tests::perft(board, 4, false) << "\n"; // should be 4085603

    return 0;
}