#pragma once
#include "move.h"
#include "util.h"
#include <cstdint>
#include <vector>

namespace BBD::Engine
{

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

    inline int entry_depth(uint64_t key) const
    {
        return table[index_of(key)].depth;
    }

    inline size_t index_of(uint64_t key) const
    {
        return static_cast<size_t>(key & (TT_SIZE - 1ULL));
    }

    bool probe(uint64_t key, int depth, Score &out_score, TTBound &out_bound, Move &out_move)
    {
        TTEntry &entry = table[index_of(key)];
        if (entry.key == key)
        {
            out_score = entry.score;
            out_bound = entry.bound;
            out_move = entry.best_move;
            return true;
        }
        return false;
    }

    // Store a new entry in TT
    void store(uint64_t key, int depth, Score score, TTBound bound, Move best_move)
    {
        TTEntry &entry = table[index_of(key)];
        entry.key = key;
        entry.depth = depth;
        entry.score = score;
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

} // namespace BBD::Engine
