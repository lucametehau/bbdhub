#include "../tests/test_utils.h"
#include "attacks.h"
#include "board.h"
#include "move.h"
#include "piece.h"
#include "search.h"
#include "square.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Engine;

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
    using namespace BBD::Tests;
    SearchLimiter limiter;
    limiter.set_depth(6);
    SearchThread thread;

    Board board("4k3/8/4K3/8/4Q3/8/8/8 w - - 0 1");
    print_board(board);
    std::cout << thread.search(board, limiter).to_string() << "\n";

    return 0;
}