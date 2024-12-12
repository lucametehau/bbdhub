Current progress

- Implemented CMake file
- Reviewed initial Board Class pull request 
- Implemented Zobrist hashing (zobrist.h)
- Implemeneted castling rights management in board.h
- Implemented BoardState class in board.h which keeps track of castling rights, en passant target squares and captured pieces altogether. This allowed us to replace captutred_pieces array by vector<BoardState> board_state_array and then implement move generation and other important functions
- Fixed some bugs

Last update: December 12th, 2024