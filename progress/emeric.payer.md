# My Progress

## Week-by-week progress with short explanations

#### Week 1

Implemented the basic classes for the board, including:
- Square class (square.h): we represent the board as an array of 64 values ranging from 0 to 63, where each value represents a square on the board
- Color class (color.h)

### Week 2

Implemented the Bitboard class (bitboard.h)
- Represents an 8x8 chessboard as a 64-bit integer 
- Provides methods and operators for manipulating the bitboard efficiently

#### Week 3

To enter the leaderboard we had to be able to read the input file and write our move on an output file, so I implemented and tested argument parser (parsing.h)
- Recreate a Board state by reading and applying the sequence of moves from the input file
- Add the played move to the output file

#### Week 4

Now that our chess engine is operational, our main focus has shifted to improving its performance, notably its *search* and *evaluation* capabilities.

Improved the *search* capability by implementing principal variation search, enhancing alpha-beta pruning (search.h) previously implemented
- Assume the first move evaluated in a node is the best, narrowing the alpha-beta window for subsequent moves
- Reduces the search space to make more searches

#### Week 5

This week I continued to improve our engine, notably it's *search* capability, notably:
- Implemented reverse futility pruning (to evaluate a move based on the material balance, avoiding deeper search if a move seems unlikely to improve the score significantly)
- Started implementing transposition tables

#### Week 6

Continued working on the *search* capability of the engine:
- Finished implemeting, tested and merged transposition tables (tt.h)

🔄 In Progress

Last update: January 13th, 2025





