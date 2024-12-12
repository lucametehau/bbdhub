#include "board.h"
#include "attacks.h"

namespace BBD
{

const Bitboard Board::get_pinned_pieces() const
{
    const Color color = player_color(), enemy = color.flip();
    const Bitboard us = all_pieces(color), them = all_pieces(enemy);
    const Square king_square = pieces[color][PieceTypes::KING].lsb_index();
    Bitboard pinned(0ull);
    Bitboard slider_attacks =
        (attacks::generate_attacks(PieceTypes::BISHOP, king_square, them) & diagonal_sliders(enemy)) |
        (attacks::generate_attacks(PieceTypes::ROOK, king_square, them) & orthogonal_sliders(enemy));

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

const Bitboard Board::get_checkers() const
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
    Bitboard attacked(0ull);
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
        // pawn attacks
        attacked = get_pawn_attacks(enemy);

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
    int checkers_count = checkers().count();

    if (checkers_count == 2)
    {
        // only king moves allowed in double check
        return nr_moves;
    }
    else if (checkers_count == 1)
    {
        noisy_mask = checkers();
        // can't do any non-king quiet move if we have a knight checking
        quiet_mask = at(checkers().lsb_index()).type() == PieceTypes::KNIGHT
                         ? Bitboard(0ull)
                         : attacks::between_mask[king_square][checkers().lsb_index()];
    }
    else
    {
        noisy_mask = them;
        quiet_mask = empty;
    }

    const Bitboard pinned = pinned_pieces();

    const int rank7 = color == Colors::WHITE ? 6 : 1, rank3 = color == Colors::WHITE ? 2 : 5;
    const int file_a = color == Colors::WHITE ? 0 : 7, file_h = 7 - file_a;
    const Bitboard pawns = pieces[color][PieceTypes::PAWN];
    const Bitboard non_promo_pawns = pawns & ~attacks::rank_mask[rank7],
                   promo_pawns = pawns & attacks::rank_mask[rank7];

    // pawn pushes
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

    // pawn captures
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

    // en passant
    {
        Square ep = get_en_passant_square();
        if (ep != Squares::NO_SQUARE)
        {
            Bitboard ep_pawns = non_promo_pawns & attacks::generate_attacks_pawn(enemy, ep);

            while (ep_pawns)
            {
                Square sq = ep_pawns.lsb_index();
                moves[nr_moves++] = Move(sq, ep, MoveTypes::ENPASSANT);
                ep_pawns ^= Bitboard(sq);
            }
        }
    }

    // promotions
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

    // knights, only unpinned
    Bitboard mask = pieces[color][PieceTypes::KNIGHT] & ~pinned;
    while (mask)
    {
        Square sq = mask.lsb_index();
        nr_moves =
            add_moves(moves, sq, attacks::generate_attacks(PieceTypes::KNIGHT, sq, occ) & (quiet_mask | noisy_mask));
        mask ^= Bitboard(sq);
    }

    // all other pieces
    for (PieceType pt = PieceTypes::BISHOP; pt <= PieceTypes::QUEEN; pt++)
    {
        mask = pieces[color][pt];
        while (mask)
        {
            Square sq = mask.lsb_index();
            Bitboard attacks = attacks::generate_attacks(pt, sq, occ) & (quiet_mask | noisy_mask);
            // a slider can only move on the squares on the pin
            if (pinned.has_square(sq))
                attacks &= attacks::line_mask[king_square][sq];
            nr_moves = add_moves(moves, sq, attacks);
            mask ^= Bitboard(sq);
        }
    }

    // castling
    {
        auto has_castling_right = [&](const int castle_rights, const Color color, const int side) {
            return color == Colors::WHITE ? (castle_rights >> side) & 1 : (castle_rights >> (2 + side)) & 1;
        };
        // king side
        if (has_castling_right(get_castling_rights(), color, 0))
        {
            Bitboard b = attacks::between_mask[king_square][king_square + 3];
            if (!(attacked & (Bitboard(king_square) | b)) && !(occ & b))
                moves[nr_moves++] = Move(king_square, king_square + 2, MoveTypes::CASTLE);
        }
        if (has_castling_right(get_castling_rights(), color, 1))
        {
            Bitboard b = attacks::between_mask[king_square][king_square - 3];
            if (!(attacked & (Bitboard(king_square) | b)) && !(occ & (Bitboard(king_square - 3) | b)))
                moves[nr_moves++] = Move(king_square, king_square - 2, MoveTypes::CASTLE);
        }
    }
    return nr_moves;
}

// only needed for pawn moves really
bool Board::is_legal(Move move) const
{
    const Square from = move.from(), to = move.to();
    const Piece piece = at(from);
    const Square king_square = pieces[player_color()][PieceTypes::KING].lsb_index();

    // specifically for pawns, they need to move on the checking line
    if (!checkers())
    {
        if (attacks::line_mask[from][to].has_square(king_square))
            return true;
    }

    // check if doing the enpassant gets us in check
    if (move.type() == MoveTypes::ENPASSANT)
    {
        const Color color = player_color(), enemy = color.flip();
        const Square sq = to.shift<SOUTH>(color);
        const Bitboard all_no_ep =
            (all_pieces(Colors::WHITE) | all_pieces(Colors::BLACK)) ^ Bitboard(from) ^ Bitboard(to) ^ Bitboard(sq);
        return !(attacks::generate_attacks_rook(king_square, all_no_ep) & orthogonal_sliders(enemy)) &&
               !(attacks::generate_attacks_bishop(king_square, all_no_ep) & diagonal_sliders(enemy));
    }

    // if not on the checking line, it shouldn't be pinned
    if (piece.type() == PieceTypes::PAWN)
        return !pinned_pieces().has_square(from);

    return true;
}

}; // namespace BBD