#include "search.h"
#include "eval.h"
#include <cassert>

namespace BBD::Engine
{

void SearchThread::order_moves(MoveList &moves, int nr_moves)
{
    std::array<int, 256> scores;

    // try to rank captures higher
    for (int i = 0; i < nr_moves; i++)
    {
        Move move = moves[i];
        if (board.is_capture(move))
            scores[i] = 1000 * board.at(move.to());
        else
            scores[i] = 0;
    }

    // simple sort
    for (int i = 0; i < nr_moves; i++)
    {
        for (int j = i + 1; j < nr_moves; j++)
        {
            if (scores[j] > scores[i])
            {
                std::swap(scores[j], scores[i]);
                std::swap(moves[j], moves[i]);
            }
        }
    }
}

Score SearchThread::quiescence(Score alpha, Score beta)
{
    if (board.three_fold_repetition_check()) {
        return 0; // draw
    }
    
    nodes++;
    if (limiter.get_mode() == SearchLimiter::SearchMode::TIME_SEARCH)
    {
        if (nodes && nodes % (1 << 12))
        {
            if (get_time_since_start() - start_time > limiter.get_move_time())
                throw "Timeout";
        }
    }

    Score eval = board_evaluation(board) * (board.player_color() == Colors::WHITE ? 1 : -1);
    Score best = eval;

    if (best >= beta)
        return best;
    alpha = std::max(alpha, best);

    MoveList moves;
    int nr_moves = board.gen_legal_moves<CAPTURE_MOVES>(moves);

    order_moves(moves, nr_moves);

    for (int i = 0; i < nr_moves; i++)
    {
        Move move = moves[i];

        if (!board.is_legal(move))
            continue;

        assert(board.is_capture(move));

        board.make_move(move);
        Score score = -quiescence(-beta, -alpha);
        board.undo_move(move);

        if (score > best)
        {
            best = score;
            if (score > alpha)
            {
                alpha = score;
                if (alpha >= beta)
                    break;
            }
        }
    }
    return best;
}

template <bool root_node> Score SearchThread::negamax(Score alpha, Score beta, int depth, int ply)
{
    if (!root_node && board.three_fold_repetition_check()) {
        return 0; // draw
    }

    if (depth == 0)
        return quiescence(alpha, beta);
    nodes++;

    if (limiter.get_mode() == SearchLimiter::SearchMode::TIME_SEARCH)
    {
        if (nodes && nodes % (1 << 12))
        {
            if (get_time_since_start() - start_time > limiter.get_move_time())
                throw "Timeout";
        }
    }

    MoveList moves;
    int nr_moves = board.gen_legal_moves<ALL_MOVES>(moves);

    order_moves(moves, nr_moves);

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

    start_clock();
    auto depth = 1;
    auto running = true;
    int limit_depth = limiter.get_mode() == SearchLimiter::SearchMode::DEPTH_SEARCH ? limiter.get_depth() : 100;
    while (running && depth <= limit_depth) /// limit how much we can search
    {
        try
        {
            score = negamax<true>(-INF, INF, depth, 0);
            std::cout << "info score " << score << " depth " << depth << " nodes " << nodes << " time "
                      << get_time_since_start() - search_start_time << std::endl;
            depth++;
        }
        catch (...)
        {
            running = false;
        }
    }
    std::cout << "bestmove " << thread_best_move.to_string() << std::endl;

    return thread_best_move;
}

} // namespace BBD::Engine