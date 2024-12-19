#include "search.h"
#include <cassert>

namespace BBD::Engine
{

template <bool root_node> int SearchThread::negamax(int alpha, int beta, int depth, int ply)
{
    nodes++;
    if (depth == 0)
        return 0; // replace with eval when time is

    MoveList moves;
    int nr_moves = board.gen_legal_moves(moves);

    Score best = -INF;
    int played = 0;
    for (int i = 0; i < nr_moves; i++)
    {
        Move move = moves[i];
        if (!board.is_legal(move))
            continue;

        board.make_move(move);
        played++;
        int score = -negamax<false>(-beta, -alpha, depth - 1, ply + 1);
        board.undo_move(move);

        if (score > best)
        {
            best = score;

            if (score > alpha)
            {
                if constexpr (root_node)
                {
                    thread_best_move = move;
                    thread_best_score = score;
                }
                alpha = score;
                if (alpha >= beta)
                    break;
            }
        }
    }

    // check for checkmate or stalemate
    if (played == 0)
        return board.checkers() ? -INF + ply : 0;

    return best;
}

Move SearchThread::search(Board &_board, SearchLimiter &_limiter)
{
    auto search_start_time = get_time_since_start();
    nodes = 0;
    board = _board, limiter = _limiter;
    Score score;
    if (limiter.get_mode() == SearchLimiter::SearchMode::DEPTH_SEARCH)
    {
        score = negamax<true>(-INF, INF, limiter.get_depth(), 0);
    }
    else
    {
        assert(0); // for now
    }
    std::cout << "info bestmove " << thread_best_move.to_string();
    if (abs(score) < MATE)
        std::cout << " score " << score;
    else
        std::cout << " score mate " << (score > 0 ? (INF - score + 1) / 2 : -(INF + score + 1) / 2);
    auto time_spent = get_time_since_start() - search_start_time;
    std::cout << " nodes " << nodes << " time " << time_spent << " nps " << 1000 * nodes / (time_spent + !time_spent)
              << std::endl;

    return thread_best_move;
}

} // namespace BBD::Engine