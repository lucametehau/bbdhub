#pragma once

#include "board.h"
#include "move.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace BBD
{

// First we read the history of moves from the input file
Board read_move_history(const std::string &filename)
{
    std::vector<BBD::Move> move_history;
    std::ifstream input_file(filename);

    if (!input_file.is_open())
    {
        std::cerr << "Error: could not open file " << filename << std::endl;
        exit(0);
    }

    Board board;

    std::string move_str;
    while (std::getline(input_file, move_str))
    {
        if (!move_str.empty())
        {
            BBD::MoveList legal_moves;
            int legal_count = board.gen_legal_moves<ALL_MOVES>(legal_moves);

            for (int i = 0; i < legal_count; ++i)
            {
                if (legal_moves[i].to_string() == move_str)
                {
                    board.make_move(legal_moves[i]);
                    break;
                }
            }
        }
    }

    input_file.close();

    return board;
}

// Then we append the next move to the output file
void append_move_to_file(const std::string &filename, const std::string &move)
{
    std::ofstream output_file(filename);
    if (!output_file.is_open())
    {
        std::cerr << "Error: could not open file " << filename << std::endl;
        exit(0);
    }

    output_file << move << "\n";
    output_file.close();
}

} // namespace BBD
