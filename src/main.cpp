#include "attacks.h"
#include "board.h"
#include "move.h"
#include "piece.h"
#include "square.h"
#include <iostream>

using namespace BBD; // DON'T LEAVE THIS HERE

uint64_t perft(Board &board, int depth, bool root = true)
{
    if (depth == 0)
        return 1;
    MoveList moves;
    int nr_moves = board.gen_legal_moves(moves);

    uint64_t nodes = 0;
    for (int i = 0; i < nr_moves; i++)
    {
        Move move = moves[i];
        if (!board.is_legal(move))
            continue;

        board.make_move(move);
        uint64_t x = perft(board, depth - 1, false);
        if (root)
            std::cout << move.to_string() << " : " << x << "\n";
        nodes += x;
        board.undo_move(move);
    }
    return nodes;
}

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
    Board board;
    // board.make_move(Move(C2, C3, MoveTypes::NO_TYPE));
    // board.make_move(Move(A7, A6, MoveTypes::NO_TYPE));
    std::cout << perft(board, 4) << "\n";
    return 0;
}