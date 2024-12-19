#pragma once
#include "search.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

namespace BBD::Engine::UCI
{

void uci_loop()
{
    std::cout << "bbd by a team of very nice people!" << std::endl;
    std::cout << "info string Warning! This is UCI mode, not the usual tournanment mode!" << std::endl;
    Board board;
    SearchLimiter limiter;
    SearchThread thread;
    limiter.set_depth(6);

    std::string input;
    while (getline(std::cin, input))
    {
        std::istringstream iss(input);
        std::string command;
        iss >> std::skipws >> command;

        if (command == "uci")
        {
            std::cout << "id name bbd" << std::endl;
            std::cout << "id author cool people" << std::endl;

            std::cout << "uciok" << std::endl;
        }
        else if (command == "go")
        {
            std::string parameter;
            uint64_t time = 0, inc = 0;

            while (iss >> parameter) {
                if (parameter == "wtime" && board.player_color() == Colors::WHITE) {
                    iss >> time;
                }
                else if (parameter == "btime" && board.player_color() == Colors::BLACK) {
                    iss >> time;
                }
                else if (parameter == "winc" && board.player_color() == Colors::WHITE) {
                    iss >> inc;
                }
                else if (parameter == "binc" && board.player_color() == Colors::BLACK) {
                    iss >> inc;
                }
                else if (parameter == "depth") {
                    int depth;
                    iss >> depth;
                    limiter.set_depth(depth);
                }
            }
            if (time || inc)
                limiter.set_time(time / 20 + inc / 2);
            
            thread.search(board, limiter);
        }
        else if (command == "position")
        {
            std::string position_type;
            while (iss >> position_type)
            {
                if (position_type == "startpos")
                {
                    board.set_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                }
                else if (position_type == "fen")
                {
                    std::string fen;
                    for (int i = 0; i < 6; i++)
                    {
                        std::string temp;
                        iss >> temp;
                        fen += temp + " ";
                    }
                    board.set_fen(fen);
                }
                else if (position_type == "moves")
                {
                    std::string move_str;
                    while (iss >> move_str)
                    {
                        MoveList moves;
                        int nr_moves = board.gen_legal_moves(moves);
                        bool legal_move = false;

                        for (int i = 0; i < nr_moves; i++)
                        {
                            if (!board.is_legal(moves[i]))
                                continue;

                            if (moves[i].to_string() == move_str)
                            {
                                board.make_move(moves[i]);
                                legal_move = true;
                                break;
                            }
                        }
                        if (!legal_move)
                        {
                            std::cout << "info got illegal move " << move_str << std::endl;
                            break;
                        }
                    }
                }
            }
        }
        else if (command == "isready")
        {
            std::cout << "readyok" << std::endl;
        }
        else if (command == "quit")
        {
            exit(0);
        }
    }
}

} // namespace BBD::Engine::UCI