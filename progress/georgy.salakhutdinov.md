Current progress

- Implemented CMake file
- Reviewed initial Board Class pull request 
- Implemented Zobrist hashing (zobrist.h)
- Implemeneted castling rights management in board.h (updating after make_move and after undo_move)
- Implemented BoardState class in board.h which keeps track of castling rights, en passant target squares and captured pieces altogether. This allowed me to replace captutred_pieces array by board_state_array and then update castling rights and en passant target square after each make_move and afer each undo_move, and then implement move generation and other important functions
- Fixed some bugs

Last update: December 12th, 2024