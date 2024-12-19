#pragma once
#include "search.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

namespace BBD::Engine::UCI {

void uci_loop() {
    std::cout << "BBD by a team of very nice people!" << std::endl;
    std::cout << "info string Warning! This is UCI mode, not the usual tournanment mode!" << std::endl;
    Board board;
    SearchLimiter limiter;
    SearchThread thread;
    limiter.set_depth(6);

    std::string input;
    while (getline(std::cin, input)) {
        std::istringstream iss(input);
        std::string command;
        iss >> std::skipws >> command;

        if (command == "go") {
            thread.search(board, limiter);
        }
        else if (command == "position") {
            std::string position_type;
            while (iss >> position_type) {
                if (position_type == "startpos") {
                    board.set_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                }
                else if (position_type == "fen") {
                    std::string fen;
                    for (int i = 0; i < 6; i++) {
                        std::string temp;
                        iss >> temp;
                        fen += temp + " ";
                    }
                    board.set_fen(fen);
                }
                else if (position_type == "moves") {
                    std::string move_str;
                    while (iss >> move_str) {
                        MoveList moves;
                        int nr_moves = board.gen_legal_moves(moves);
                        bool legal_move = false;

                        for (int i = 0; i < nr_moves; i++) {
                            if (!board.is_legal(moves[i])) continue;

                            if (moves[i].to_string() == move_str) {
                                board.make_move(moves[i]);
                                legal_move = true;
                                break;
                            }
                        }
                        if (!legal_move) {
                            std::cout << "info got illegal move " << move_str << std::endl;
                            break;
                        }
                    }
                }
            }
        }
        else if (command == "quit") {
            exit(0);
        }
    }
}

}