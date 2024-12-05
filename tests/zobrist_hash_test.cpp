#include <iostream>
#include <cassert>
#include "../src/board.h"
#include "../src/move.h"
#include "../src/zobrist.h"

using namespace BBD;


void print_board(const Board& board) {
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << rank + 1 << "  ";
        for (int file = 0; file < 8; file++) {
            Piece piece = board.at(rank * 8 + file);
            if (piece != Pieces::NO_PIECE) {
                std::cout << piece.to_char() << ' ';
            } else {
                std::cout << ". ";
            }
        }
        std::cout << '\n';
    }
    std::cout << "\n   a b c d e f g h\n\n";
}

void test1() {
    Zobrist::init();
    Board board;
    print_board(board);
    uint64_t initial_hash = Zobrist::hash_calc(board);
    std::cout << "Initial hash: " << initial_hash << "\n";

    Move bmove1(Squares::G1, Squares::F3, NO_TYPE);
    //Move bmove2(Squares::F7, Squares::F4, NO_TYPE);
    Move bmove3(Squares::F3, Squares::G1, NO_TYPE);
    board.make_move(bmove1);
    //board.make_move(bmove2);
    board.make_move(bmove3);
    print_board(board);

    uint64_t new_hash = Zobrist::hash_calc(board);
    std::cout << "New hash: " << new_hash << "\n";

}



int main() {
    std::cout << "Starting hash tests...\n\n";

    test1();

    //std::cout << "All tests passed!\n";
    return 0;
}