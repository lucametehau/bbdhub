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

    Board current_board = read_move_history(argv[2]);

    std::string next_move_str = get_next_move(current_board);
    append_move_to_file(argv[4], next_move_str);

    using namespace BBD::Tests;
    SearchLimiter limiter;
    limiter.set_depth(6);
    SearchThread thread;

    Board board("4k3/8/4K3/8/4Q3/8/8/8 w - - 0 1");
    print_board(board);
    thread.search(board, limiter);

    Board board2("4k3/8/4K3/5Q2/8/8/8/8 w - - 0 1");
    print_board(board2);
    thread.search(board2, limiter);

    Board board3("4k3/8/4K3/4R3/8/8/8/8 w - - 0 1");
    print_board(board3);
    thread.search(board3, limiter);

    return 0;
}