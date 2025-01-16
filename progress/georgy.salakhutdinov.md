Current progress

- Implemented CMake file
- Reviewed initial Board Class pull request 
- Implemented and tested Zobrist hashing (zobrist.h and board.h)
- Implemeneted castling rights management in board.h (updating after make_move and after undo_move)
- Implemented BoardState class in board.h which keeps track of castling rights, en passant target squares, captured pieces, zobrist hash, checkers and pinned_pieces altogether. This allowed me to replace captutred_pieces array by board_state_array and then quickly update castling rights and en passant target square after each make_move and after each undo_move, and then implement move generation and other important functions
- Fixed some bugs
- Implemented and tested basic evaluation of the board (eval.h and evaluation_tests.cpp)
- Implemented and tested 3-fold checking (board.h and threefold_test.cpp)
- Fixed build errors in uci.h
- Implemented and tested zobrist hash inctremental updates after each move (in board.h make_move()), which led to a massive improvement. Implemented tests for it (inctremental_hash_calc_test.cpp)
- Fixed bugs in set_fen() in board.h

Last update: January 16th, 2025