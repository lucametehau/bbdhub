Current progress

- Implemented CMake file
- Reviewed initial Board Class pull request 
- Implemented Zobrist hashing (zobrist.h and zobrist.cpp)
- Implemeneted castling rights management in board.h (updating after make_move and after undo_move)
- Implemented BoardState class in board.h which keeps track of castling rights, en passant target squares and captured pieces altogether. This allowed me to replace captutred_pieces array by board_state_array and then update castling rights and en passant target square after each make_move and afer each undo_move, and then implement move generation and other important functions
- Fixed some bugs
- Implemented evaluation of the board (eval.h) and tests for it (evaluation_tests.cpp)
- Implemented inctremental updates of the zobrist hash of the board after each move (in board.h make_move()), which improved the efficiency since now we don't have to re-calcualte the whole hash at each step. Also implemented tests for it (zobrist_test.cpp)
- Fixed build errors in uci.h

Last update: January 6th, 2025