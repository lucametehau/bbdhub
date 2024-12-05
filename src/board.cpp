#include "board.h"
#include "attacks.h"

namespace BBD
{

Bitboard Board::get_pinned_pieces() const
{
    const Color color = player_color(), enemy = color.flip();
    const Bitboard us = all_pieces(color), them = all_pieces(enemy);
    const Square king_square = pieces[color][PieceTypes::KING].lsb_index();
    Bitboard pinned(0ull);
    Bitboard slider_attacks = (attacks::generate_attacks(PieceTypes::BISHOP, king_square, them) &
                               (pieces[enemy][PieceTypes::BISHOP] | pieces[enemy][PieceTypes::QUEEN])) |
                              (attacks::generate_attacks(PieceTypes::ROOK, king_square, them) &
                               (pieces[enemy][PieceTypes::ROOK] | pieces[enemy][PieceTypes::QUEEN]));

    while (slider_attacks)
    {
        Square sq = slider_attacks.lsb_index();
        Bitboard pieces_on_ray = us & attacks::between_mask[sq][king_square];
        if (pieces_on_ray.count() == 1)
            pinned |= pieces_on_ray;
        slider_attacks ^= Bitboard(sq);
    }
    return pinned;
}

Bitboard Board::get_checkers() const
{
    const Color color = player_color(), enemy = color.flip();
    const Square king_square = pieces[color][PieceTypes::KING].lsb_index();
    const Bitboard occ = all_pieces(color) | all_pieces(enemy);
    Bitboard attacks = pieces[enemy][PieceTypes::PAWN] & attacks::generate_attacks_pawn(color, king_square);
    for (PieceType pt = PieceTypes::KNIGHT; pt <= PieceTypes::KING; pt++)
        attacks |= pieces[enemy][pt] & attacks::generate_attacks(pt, king_square, occ);

    return attacks;
}

int Board::gen_legal_moves(MoveList &moves)
{
    const Color color = player_color(), enemy = color.flip();
    const Square king_square = pieces[color][PieceTypes::KING].lsb_index();
    Bitboard us = all_pieces(color), them = all_pieces(enemy), occ = us | them, empty = ~occ;
    int nr_moves = 0;

    auto add_moves = [&](MoveList &moves, Square sq, Bitboard mask) {
        while (mask)
        {
            Square sq_to = mask.lsb_index();
            moves[nr_moves++] = Move(sq, sq_to, MoveTypes::NO_TYPE);
            mask ^= Bitboard(sq_to);
        }
        return nr_moves;
    };

    // king moves
    {
        Bitboard attacked(0ull);
        // pawn attacks
        attacked = 0;

        for (PieceType pt = PieceTypes::KNIGHT; pt <= PieceTypes::KING; pt++)
        {
            Bitboard mask = pieces[enemy][pt];
            while (mask)
            {
                Square sq = mask.lsb_index();
                attacked |= attacks::generate_attacks(pt, sq, occ ^ Bitboard(king_square));
                mask ^= Bitboard(sq);
            }
        }
        nr_moves = add_moves(moves, king_square,
                             attacks::generate_attacks(PieceTypes::KING, king_square, occ) & ~(us | attacked));
    }

    Bitboard noisy_mask, quiet_mask;
    Bitboard checkers = get_checkers();
    int checkers_count = checkers.count();

    if (checkers_count == 2)
    {
        // only king moves allowed in double check
        return nr_moves;
    }
    else if (checkers_count == 1)
    {
        noisy_mask = checkers;
        quiet_mask = at(checkers.lsb_index()).type() == PieceTypes::KNIGHT
                         ? Bitboard(0ull)
                         : attacks::between_mask[king_square][checkers.lsb_index()];

        // enpassant to cancel check
    }
    else
    {
        noisy_mask = them;
        quiet_mask = empty;

        // enpassant stuff again

        // castling
    }

    const Bitboard pinned = get_pinned_pieces();

    const int rank7 = color == Colors::WHITE ? 6 : 1, rank3 = color == Colors::WHITE ? 2 : 5;
    const int file_a = color == Colors::WHITE ? 0 : 7, file_h = 7 - file_a;
    const Bitboard pawns = pieces[color][PieceTypes::PAWN];
    const Bitboard non_promo_pawns = pawns & ~attacks::rank_mask[rank7],
                   promo_pawns = pawns & attacks::rank_mask[rank7];

    {
        Bitboard single_push = non_promo_pawns.shift<NORTH>(color) & empty;
        Bitboard double_push = (single_push & attacks::rank_mask[rank3]).shift<NORTH>(color) & empty & quiet_mask;
        single_push &= quiet_mask;

        while (single_push)
        {
            Square sq = single_push.lsb_index();
            moves[nr_moves++] = Move(sq.shift<SOUTH>(color), sq, MoveTypes::NO_TYPE);
            single_push ^= Bitboard(sq);
        }

        while (double_push)
        {
            Square sq = double_push.lsb_index();
            moves[nr_moves++] = Move(sq.shift<SOUTH_SOUTH>(color), sq, MoveTypes::NO_TYPE);
            double_push ^= Bitboard(sq);
        }
    }

    {
        Bitboard west_captured = (non_promo_pawns & ~attacks::file_mask[file_a]).shift<NORTHWEST>(color) & noisy_mask;
        Bitboard east_captured = (non_promo_pawns & ~attacks::file_mask[file_h]).shift<NORTHEAST>(color) & noisy_mask;

        while (west_captured)
        {
            Square sq = west_captured.lsb_index();
            moves[nr_moves++] = Move(sq.shift<SOUTHEAST>(color), sq, MoveTypes::NO_TYPE);
            west_captured ^= Bitboard(sq);
        }

        while (east_captured)
        {
            Square sq = east_captured.lsb_index();
            moves[nr_moves++] = Move(sq.shift<SOUTHWEST>(color), sq, MoveTypes::NO_TYPE);
            east_captured ^= Bitboard(sq);
        }
    }

    {
        Bitboard west_promo = (promo_pawns & ~attacks::file_mask[file_a]).shift<NORTHWEST>(color) & noisy_mask;
        Bitboard east_promo = (promo_pawns & ~attacks::file_mask[file_h]).shift<NORTHEAST>(color) & noisy_mask;
        Bitboard quiet_promo = promo_pawns.shift<NORTH>(color) & quiet_mask;

        auto add_promotions = [&](MoveList &moves, Square sq, Square sq_to) {
            moves[nr_moves++] = Move(sq, sq_to, MoveTypes::PROMO_KNIGHT);
            moves[nr_moves++] = Move(sq, sq_to, MoveTypes::PROMO_BISHOP);
            moves[nr_moves++] = Move(sq, sq_to, MoveTypes::PROMO_ROOK);
            moves[nr_moves++] = Move(sq, sq_to, MoveTypes::PROMO_QUEEN);
            return nr_moves;
        };

        while (quiet_promo)
        {
            Square sq = quiet_promo.lsb_index();
            nr_moves = add_promotions(moves, sq.shift<SOUTH>(color), sq);
            quiet_promo ^= Bitboard(sq);
        }

        while (west_promo)
        {
            Square sq = west_promo.lsb_index();
            nr_moves = add_promotions(moves, sq.shift<SOUTHEAST>(color), sq);
            west_promo ^= Bitboard(sq);
        }

        while (east_promo)
        {
            Square sq = east_promo.lsb_index();
            nr_moves = add_promotions(moves, sq.shift<SOUTHWEST>(color), sq);
            east_promo ^= Bitboard(sq);
        }
    }

    Bitboard mask = pieces[color][PieceTypes::KNIGHT] & ~pinned;
    while (mask)
    {
        Square sq = mask.lsb_index();
        nr_moves =
            add_moves(moves, sq, attacks::generate_attacks(PieceTypes::KNIGHT, sq, occ) & (quiet_mask | noisy_mask));
        mask ^= Bitboard(sq);
    }

    for (PieceType pt = PieceTypes::BISHOP; pt <= PieceTypes::QUEEN; pt++)
    {
        mask = pieces[color][pt];
        while (mask)
        {
            Square sq = mask.lsb_index();
            Bitboard attacks = attacks::generate_attacks(pt, sq, occ) & (quiet_mask | noisy_mask);
            if (pinned.has_square(sq))
                attacks &= attacks::line_mask[king_square][sq];
            nr_moves = add_moves(moves, sq, attacks);
            mask ^= Bitboard(sq);
        }
    }
    return nr_moves;
}

bool Board::is_legal(Move move) const
{
    const Square from = move.from(), to = move.to();
    const Piece piece = at(from);
    const Square king_square = pieces[player_color()][PieceTypes::KING].lsb_index();

    if (!get_checkers())
    {
        if (attacks::line_mask[from][to].has_square(king_square))
            return true;
    }

    if (piece.type() == PieceTypes::PAWN)
        return !get_pinned_pieces().has_square(from);

    return true;
}

}; // namespace BBD