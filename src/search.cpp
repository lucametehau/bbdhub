#include "search.h"
#include <cassert>
#include <fstream>
#include <iostream>

namespace BBD::Engine
{

inline void print_board(const Board &board)
{
    for (int rank = 7; rank >= 0; rank--)
    {
        std::cout << rank + 1 << "  ";
        for (int file = 0; file < 8; file++)
        {
            Piece piece = board.at(rank * 8 + file);
            if (piece != Pieces::NO_PIECE)
            {
                std::cout << piece.to_char() << ' ';
            }
            else
            {
                std::cout << ". ";
            }
        }
        std::cout << '\n';
    }
    std::cout << "\n   a b c d e f g h\n\n";
}

void SearchThread::order_moves(MoveList &moves, int nr_moves, const Move tt_move, int ply)
{
    std::array<int, 256> scores;

    // Try to rank captures higher
    for (int i = 0; i < nr_moves; i++)
    {
        Move move = moves[i];

        if (move == tt_move)
        {
            // Give the TT move a huge bonus so it sorts first
            scores[i] = 1000000000;
        }
        else if (board.is_capture(move))
        {
            scores[i] = 900000000 + 100000 * int(board.at(move.to()));
        }
        else if (move == killers[ply][0])
        {
            scores[i] = 800000000;
        }
        else if (move == killers[ply][1])
        {
            scores[i] = 700000000;
        }
        else
        {
            scores[i] = history[board.player_color()][move.from()][move.to()];
        }
    }

    // Simple sort
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

Score SearchThread::quiescence(Score alpha, Score beta, int ply)
{
    if (board.threefold_check())
    {
        return 0; // draw
    }

    if (ply == MAX_DEPTH)
    { // don't pass the maximum depth, might crash
        return NNUE::NNUENetwork::evaluate(board.get_accumulators(), board.player_color());
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

    Score eval = NNUE::NNUENetwork::evaluate(board.get_accumulators(), board.player_color());
    Score best = eval;

    if (best >= beta)
        return best;
    alpha = std::max(alpha, best);

    MoveList moves;
    int nr_moves = board.gen_legal_moves<CAPTURE_MOVES>(moves);

    order_moves(moves, nr_moves, NULL_MOVE, ply);

    for (int i = 0; i < nr_moves; i++)
    {
        Move move = moves[i];

        if (!board.is_legal(move))
            continue;

        assert(board.is_capture(move));

        board.make_move(move);
        Score score = -quiescence(-beta, -alpha, ply + 1);
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
    Score alpha_original = alpha;
    Move best_move;
    bool in_check = board.checkers();

    if (!root_node && board.threefold_check())
    {
        return 0; // draw
    }
    if (depth <= 0)
        return quiescence(alpha, beta, ply);

    nodes++;

    if (limiter.get_mode() == SearchLimiter::SearchMode::TIME_SEARCH)
    {
        if (nodes && nodes % (1 << 12))
        {
            if (get_time_since_start() - start_time > limiter.get_move_time())
                throw "Timeout";
        }
    }

    // Transposition table probe
    uint64_t pos_key = board.get_cur_hash();
    Move tt_move = NULL_MOVE;

    {
        Score tt_score;
        TTBound tt_bound;

        if (tt.probe(pos_key, depth, tt_score, tt_bound, tt_move))
        {
            if (!root_node && tt.entry_depth(pos_key) >= depth)
            {
                if (tt_bound == TTBound::EXACT)
                    return tt_score;
                if (tt_bound == TTBound::LOWER && tt_score > alpha)
                    alpha = tt_score;
                else if (tt_bound == TTBound::UPPER && tt_score < beta)
                    beta = tt_score;

                if (alpha >= beta)
                    return tt_score;
            }
        }
    }

    // Reverse futility pruning
    Score eval = NNUE::NNUENetwork::evaluate(board.get_accumulators(), board.player_color());

    if (!root_node && !board.checkers() && depth <= 3)
    {
        int margin = 200 * depth;
        if (eval >= beta + margin)
        {
            return eval;
        }
    }

    // check for null-move
    {
        auto major_pieces = board.get_piece_bitboard(board.player_color(), PieceTypes::QUEEN) |
                            board.get_piece_bitboard(board.player_color(), PieceTypes::ROOK);

        auto non_pawn_material = major_pieces | board.get_piece_bitboard(board.player_color(), PieceTypes::BISHOP) |
                                 board.get_piece_bitboard(board.player_color(), PieceTypes::KNIGHT);

        const short R = 2;
        if (depth > R && !board.checkers() && major_pieces && non_pawn_material)
        {
            board.make_null_move();
            Score score = -negamax<false>(-beta, 1 - beta, depth - 1 - R, ply + 1);
            board.undo_null_move();

            if (score >= beta)
                return beta;
        }
    }

    // Principal variation search
    MoveList moves;
    int nr_moves = board.gen_legal_moves<ALL_MOVES>(moves);

    order_moves(moves, nr_moves, tt_move, ply);

    Score best = -INF;
    int played = 0;

    for (int i = 0; i < nr_moves; i++)
    {
        Move move = moves[i];
        if (!board.is_legal(move))
            continue;

        board.make_move(move);
        played++;

        Score score;

        // Late Move Reduction
        bool do_lmr = (!root_node && !in_check && !board.is_capture(move) && depth >= 2 && played > 2);

        if (do_lmr)
        {
            int reduction = (depth >= 6) ? 3 : 2;
            score = -negamax<false>(-alpha - 1, -alpha, depth - 1 - reduction, ply + 1);

            if (score > alpha && score < beta)
            {
                score = -negamax<false>(-beta, -alpha, depth - 1, ply + 1);
            }
        }
        else
        {
            score = -negamax<false>(-beta, -alpha, depth - 1, ply + 1);
        }

        /*
        if (played > 1)
        {
            score = -negamax<false>(-alpha - 1, -alpha, depth - 1, ply + 1);

            if (score > alpha && score < beta)
                score = -negamax<false>(-beta, -alpha, depth - 1, ply + 1);
        }
        else
        {
            score = -negamax<false>(-beta, -alpha, depth - 1, ply + 1);
        }
        */

        board.undo_move(move);

        if (score > best)
        {
            best = score;
            best_move = move;

            if (score > alpha)
            {
                if constexpr (root_node)
                {
                    root_best_move = move;
                }
                alpha = score;

                if (alpha >= beta)
                {
                    if (move != killers[ply][0] && move != killers[ply][1] && !board.is_capture(move))
                    {
                        killers[ply][1] = killers[ply][0];
                        killers[ply][0] = move;
                    }
                    history[board.player_color()][move.from()][move.to()] += depth * depth;
                    break;
                }
            }
        }
    }

    // Checkmate / stalemate detection
    if (played == 0)
        return board.checkers() ? -INF + ply : 0;

    // Store in transposition table
    TTBound bound_type;
    if (best <= alpha_original)
        bound_type = TTBound::UPPER;
    else if (best >= beta)
        bound_type = TTBound::LOWER;
    else
        bound_type = TTBound::EXACT;

    tt.store(pos_key, depth, best, bound_type, best_move);

    return best;
}

Move SearchThread::search(Board &_board, SearchLimiter &_limiter)
{
    auto search_start_time = get_time_since_start();
    nodes = 0;
    board = _board, limiter = _limiter;

    tt.clear();

    // Fill history with 0 at the beginning
    for (auto &t : history)
    {
        for (auto &p : t)
            p.fill(0);
    }

    for (int i = 0; i < MAX_DEPTH; i++)
    {
        killers[i].fill(NULL_MOVE);
    }

    int score, alpha, beta;
    auto depth = 1;
    auto running = true;
    int limit_depth = limiter.get_mode() == SearchLimiter::SearchMode::DEPTH_SEARCH ? limiter.get_depth() : 100;

    start_clock();
    while (running && depth <= limit_depth) // limit how much we can search
    {
        int window = 30;
        if (depth <= 4)
        {
            alpha = -INF;
            beta = INF;
        }
        else
        {
            alpha = std::max<Score>(-INF, score - window);
            beta = std::min<Score>(INF, score + window);
        }

        // aspiration windows loop
        try
        {
            while (true)
            {
                score = negamax<true>(alpha, beta, depth, 0);
                std::cout << "info score " << score << " depth " << depth << " nodes " << nodes << " time "
                          << get_time_since_start() - search_start_time << std::endl;
                std::cout << alpha << " " << beta << " " << window << "\n";
                thread_best_move = root_best_move; // only take into account full search results, for now

                if (score <= alpha)
                {
                    alpha = std::max<int>(-INF, alpha - window);
                }
                else if (score >= beta)
                {
                    beta = std::min<int>(INF, beta + window);
                }
                else
                {
                    break;
                }

                window = std::min<int>(INF, 2 * window);
            }
        }
        catch (...)
        {
            running = false;
        }

        depth++;
    }
    std::cout << "bestmove " << thread_best_move.to_string() << std::endl;

    return thread_best_move;
}

} // namespace BBD::Engine