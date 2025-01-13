#pragma once
#include "board.h"
#include "util.h"
#include <cstdint>
#include <vector>

// setup for searching thread

namespace BBD::Engine
{

inline void init()
{
    BBD::attacks::init();
    BBD::Zobrist::init();
}

enum class TTBound : uint8_t
{
    EXACT,
    LOWER,
    UPPER
};

struct TTEntry
{
    uint64_t key = 0ull;
    int depth = -1;
    Score score = 0;
    Score alpha = -30000;
    Score beta = 30000;
    TTBound bound = TTBound::EXACT;
    Move best_move;
};

// Transposition table class

class TranspositionTable
{
  private:
    static constexpr size_t TT_SIZE = 1 << 20; // can be modified!

    std::vector<TTEntry> table;

  public:
    TranspositionTable()
    {
        table.resize(TT_SIZE);
    }

    inline size_t index_of(uint64_t key) const
    {
        return static_cast<size_t>(key & (TT_SIZE - 1ULL));
    }

    bool probe(uint64_t key, int depth, Score &out_score, Score &out_alpha, Score &out_beta, TTBound &out_bound,
               Move &out_move)
    {
        TTEntry &entry = table[index_of(key)];
        if (entry.key == key)
        {
            if (entry.depth >= depth)
            {
                out_score = entry.score;
                out_alpha = entry.alpha;
                out_beta = entry.beta;
                out_bound = entry.bound;
                out_move = entry.best_move;
                return true;
            }
        }
        return false;
    }

    // Store a new entry in TT

    void store(uint64_t key, int depth, Score score, Score alpha, Score beta, TTBound bound, Move best_move)
    {
        TTEntry &entry = table[index_of(key)];
        entry.key = key;
        entry.depth = depth;
        entry.score = score;
        entry.alpha = alpha;
        entry.beta = beta;
        entry.bound = bound;
        entry.best_move = best_move;
    }

    // Clear the table

    void clear()
    {
        for (auto &e : table)
        {
            e = TTEntry();
        }
    }
};

class SearchLimiter
{
  public:
    struct SearchMode
    {
        static constexpr int DEPTH_SEARCH = 1;
        static constexpr int TIME_SEARCH = 2;
    };

  private:
    int depth;
    std::time_t move_time;
    int mode;

  public:
    void set_depth(int _depth)
    {
        depth = _depth;
        mode = SearchMode::DEPTH_SEARCH;
    }

    const int get_depth() const
    {
        return depth;
    }

    void set_time(std::time_t _move_time)
    {
        move_time = _move_time - 10;
        mode = SearchMode::TIME_SEARCH;
    }

    const std::time_t get_move_time() const
    {
        return move_time;
    }

    const int get_mode() const
    {
        return mode;
    }
};

class SearchThread
{
  private:
    Board board;
    Move thread_best_move, root_best_move;
    SearchLimiter limiter;
    std::array<std::array<std::array<int, 64>, 64>, 2> history;

    TranspositionTable tt;

    time_t start_time;

    uint64_t nodes;

  public:
    void order_moves(MoveList &moves, int nr_moves);

    Score quiescence(Score alpha, Score beta);

    template <bool root_node> Score negamax(Score alpha, Score beta, int depth, int ply);

    Move search(Board &board, SearchLimiter &limiter);

    uint64_t get_nodes()
    {
        return nodes;
    }

    void start_clock()
    {
        start_time = get_time_since_start();
    }
};

} // namespace BBD::Engine