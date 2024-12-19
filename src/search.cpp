#include "search.h"
#include "eval.h"
#include <cassert>

namespace BBD::Engine
{

template <bool root_node> int SearchThread::negamax(int alpha, int beta, int depth, int ply)
{
    nodes++;
    if (depth == 0)
        return board_evaluation(board) * (board.player_color() == Colors::WHITE ? 1 : -1);

    if (limiter.get_mode() == SearchLimiter::SearchMode::TIME_SEARCH)
    {
        if (nodes && nodes % (1 << 12))
        {
            if (get_time_since_start() - start_time > limiter.get_move_time())
                throw "Timeout";
        }
    }

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

        start_clock();
        auto depth = 1;
        auto running = true;
        while (running)
        {
            try
            {
                score = negamax<true>(-INF, INF, depth, 0);
                std::cout << "info score " << score << " depth " << depth << " nodes " << nodes << " time " << get_time_since_start() - search_start_time << std::endl;
                depth++;
            }
            catch (...)
            {
                running = false;
            }
        }

        //        assert(0); // for now
    }
    std::cout << "bestmove " << thread_best_move.to_string() << std::endl;

    return thread_best_move;
}

} // namespace BBD::Engine