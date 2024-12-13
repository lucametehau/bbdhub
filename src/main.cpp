#include "../tests/test_utils.h"
#include "attacks.h"
#include "board.h"
#include "move.h"
#include "parsing.h"
#include "piece.h"
#include "search.h"
#include "square.h"
#include <iostream>

using namespace BBD;
using namespace BBD::Engine;

int main(int argc, char *argv[])
{
    BBD::attacks::init();

    if (argc != 5)
    {
        std::cerr << "Expected 5 arguments!\n Your command should look like ./BBD - H<input history file> - m<output "
                     "move file>.";
        return 0;
    }

    Board current_board = read_move_history(argv[2], argv[4]);

    using namespace BBD::Tests;
    SearchLimiter limiter;
    limiter.set_depth(6);
    SearchThread thread;

    append_move_to_file(argv[4], thread.search(current_board, limiter).to_string());

    return 0;
}