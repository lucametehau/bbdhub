# My Progress

## Week-by-week progress with short explanations

This is a general overview of the work I’ve been doing for the past seven weeks.

#### Week 1

Implemented the square class (square.h): 
- Represent the board as an array of 64 values ranging from 0 to 63
- Each value represents a square on the board

Implemented the color class (color.h and in piece.h):
- Encapsulate a chess piece's color (black or white) using a boolean
- Modified piece.h accordingly to use the color class


### Week 2

Implemented the bitboard class (bitboard.h), fastest way to implement a chess engine:
- Represent an 8x8 chessboard as a 64-bit integer
- Provide methods and operators for manipulating the bitboard efficiently

#### Week 3

Implemented and tested argument parser (parsing.h) to enter the leaderboard, so that we can read the input file and write our move on an output file:
- Recreate a Board state by reading and applying the sequence of moves from the input file
- Add the played move to the output file

#### Week 4

Implemented and tested principal variation search (in search.cpp) to improve our engine’s search capability, enhancing alpha-beta pruning previously implemented:
- Assume the first move evaluated in a node is the best, narrowing the alpha-beta window for subsequent moves
- Reduce the search space to make more searches

#### Week 5

Implemented and tested reverse futility pruning (in search.cpp):
- Evaluate a move based on the material balance
- Avoid deeper search if a move seems unlikely to improve the score significantly

Started implementing transposition tables (tt.h and in search.h, search.cpp):
- Store previously evaluated board positions to avoid redundant computations
- Help reuse results from different move orders leading to the same position

#### Week 6

Finished implementing and tested transposition tables (tt.h and in search.h, search.cpp).

Fixed small issue with reverse futility pruning (in search.cpp).

Implemented and tested transposition table move ordering (in search.h, search.cpp, tt.h):
- Use best moves from the transposition table first during search
- Speed up pruning by exploring strong moves earlier, led to a nice improvement

#### Week 7

🔄 In Progress

Last update: January 17th, 2025





