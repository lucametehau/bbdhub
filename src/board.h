#pragma once

#include "attacks.h"
#include "bitboard.h"
#include "color.h"
#include "move.h"
#include "network.h"
#include "piece.h"
#include "square.h"
#include "zobrist.h"
#include <array>
#include <vector>

namespace BBD
{

class Board
{
  public:
    const uint8_t &get_castling_rights() const
    {
        return castling_rights;
    }

    const Square &get_en_passant_square() const
    {
        return en_passant_square;
    }

    Bitboard &pinned_pieces()
    {
        return board_state_array.back().pinned_pieces;
    }
    const Bitboard pinned_pieces() const
    {
        return board_state_array.back().pinned_pieces;
    }
    Bitboard &checkers()
    {
        return board_state_array.back().checkers;
    }
    const Bitboard checkers() const
    {
        return board_state_array.back().checkers;
    }
    const Bitboard get_piece_bitboard(Color color, PieceType p) const
    {
        return pieces[color][p];
    }
    const uint64_t get_cur_hash() const
    {
        return cur_zobrist_hash;
    }
    int get_color()
    {
        return player_color();
    }

    Board()
    {
        half_moves.clear();
        half_moves.reserve(300);
        accumulators.clear();
        accumulators.reserve(300);
        squares.fill(Pieces::NO_PIECE);
        board_state_array.clear();
        board_state_array.reserve(500);

        castling_rights = 0b1111;               // bit 0: WK, bit 1: WQ, bit 2: BK, bit 3: BQ
        en_passant_square = Squares::NO_SQUARE; // no square is available initially
        pieces[Colors::BLACK].fill(Bitboard(0ull));
        pieces[Colors::WHITE].fill(Bitboard(0ull));

        // white
        squares[Squares::A1] = Pieces::WHITE_ROOK;
        squares[Squares::B1] = Pieces::WHITE_KNIGHT;
        squares[Squares::C1] = Pieces::WHITE_BISHOP;
        squares[Squares::D1] = Pieces::WHITE_QUEEN;
        squares[Squares::E1] = Pieces::WHITE_KING;
        squares[Squares::F1] = Pieces::WHITE_BISHOP;
        squares[Squares::G1] = Pieces::WHITE_KNIGHT;
        squares[Squares::H1] = Pieces::WHITE_ROOK;

        for (Square sq = Squares::A2; sq <= Squares::H2; sq++)
        {
            squares[sq] = Pieces::WHITE_PAWN;
        }

        // black
        squares[Squares::A8] = Pieces::BLACK_ROOK;
        squares[Squares::B8] = Pieces::BLACK_KNIGHT;
        squares[Squares::C8] = Pieces::BLACK_BISHOP;
        squares[Squares::D8] = Pieces::BLACK_QUEEN;
        squares[Squares::E8] = Pieces::BLACK_KING;
        squares[Squares::F8] = Pieces::BLACK_BISHOP;
        squares[Squares::G8] = Pieces::BLACK_KNIGHT;
        squares[Squares::H8] = Pieces::BLACK_ROOK;

        for (Square sq = Squares::A7; sq <= Squares::H7; sq++)
        {
            squares[sq] = Pieces::BLACK_PAWN;
        }

        // bitmaps
        for (Square sq = 0; sq < 64; sq++)
        {
            if (!squares[sq])
                continue;
            if (squares[sq].color() == Colors::WHITE)
            {
                pieces[Colors::WHITE][squares[sq].type()].set_bit(sq, true);
            }
            else
            {
                pieces[Colors::BLACK][squares[sq].type()].set_bit(sq, true);
            }
        }

        current_color = Colors::WHITE;
        half_moves.push_back(0);

        // update land
        land = std::array<Bitboard, 2>{0ull, 0ull};
        for (PieceType p = PieceTypes::PAWN; p <= PieceTypes::KING; p++)
        {
            land[current_color] |= pieces[current_color][p];
            land[current_color.flip()] |= pieces[current_color.flip()][p];
        }

        cur_zobrist_hash = hash_calc();
        BoardState current_state{Pieces::NO_PIECE, castling_rights, en_passant_square, cur_zobrist_hash};
        board_state_array.push_back(current_state);
        checkers() = get_checkers();
        pinned_pieces() = get_pinned_pieces();
        accumulators.emplace_back();
        refresh_accumulators();
    };

    Board(const std::string &fen)
    {
        set_fen(fen);
    }

    void set_fen(const std::string &fen)
    {
        half_moves.clear();
        half_moves.reserve(300);
        accumulators.clear();
        accumulators.reserve(300);
        board_state_array.clear();
        squares.fill(Pieces::NO_PIECE);
        pieces[Colors::WHITE].fill(Bitboard(0ull));
        pieces[Colors::BLACK].fill(Bitboard(0ull));

        std::string parsing_fen = std::string(fen + "      ");
        std::string positions = parsing_fen.substr(0, fen.find(' '));
        parsing_fen = parsing_fen.substr(parsing_fen.find(' ') + 1);
        std::string color = parsing_fen.substr(0, parsing_fen.find(' '));
        parsing_fen = parsing_fen.substr(parsing_fen.find(' ') + 1);
        std::string castling = parsing_fen.substr(0, parsing_fen.find(' '));
        parsing_fen = parsing_fen.substr(parsing_fen.find(' ') + 1);
        std::string enpassant = parsing_fen.substr(0, parsing_fen.find(' '));
        parsing_fen = parsing_fen.substr(parsing_fen.find(' ') + 1);
        std::string halfmove_clock = parsing_fen.substr(0, parsing_fen.find(' '));
        parsing_fen = parsing_fen.substr(parsing_fen.find(' ') + 1);
        std::string fullmove_counter = parsing_fen.substr(0, parsing_fen.find(' '));

        // fill positions
        Square sq = Squares::A8;
        for (auto &pos : positions)
        {

            if (std::isdigit(pos))
            {
                sq = Square(sq + (pos - '0'));
                continue;
            }

            switch (pos)
            {

            case 'P':
                squares[sq] = Pieces::WHITE_PAWN;
                pieces[Colors::WHITE][PieceTypes::PAWN].set_bit(sq, true);
                break;
            case 'p':
                squares[sq] = Pieces::BLACK_PAWN;
                pieces[Colors::BLACK][PieceTypes::PAWN].set_bit(sq, true);
                break;

            case 'N':
                squares[sq] = Pieces::WHITE_KNIGHT;
                pieces[Colors::WHITE][PieceTypes::KNIGHT].set_bit(sq, true);
                break;
            case 'n':
                squares[sq] = Pieces::BLACK_KNIGHT;
                pieces[Colors::BLACK][PieceTypes::KNIGHT].set_bit(sq, true);
                break;

            case 'B':
                squares[sq] = Pieces::WHITE_BISHOP;
                pieces[Colors::WHITE][PieceTypes::BISHOP].set_bit(sq, true);
                break;
            case 'b':
                squares[sq] = Pieces::BLACK_BISHOP;
                pieces[Colors::BLACK][PieceTypes::BISHOP].set_bit(sq, true);
                break;

            case 'R':
                squares[sq] = Pieces::WHITE_ROOK;
                pieces[Colors::WHITE][PieceTypes::ROOK].set_bit(sq, true);
                break;
            case 'r':
                squares[sq] = Pieces::BLACK_ROOK;
                pieces[Colors::BLACK][PieceTypes::ROOK].set_bit(sq, true);
                break;

            case 'Q':
                squares[sq] = Pieces::WHITE_QUEEN;
                pieces[Colors::WHITE][PieceTypes::QUEEN].set_bit(sq, true);
                break;
            case 'q':
                squares[sq] = Pieces::BLACK_QUEEN;
                pieces[Colors::BLACK][PieceTypes::QUEEN].set_bit(sq, true);
                break;

            case 'K':
                squares[sq] = Pieces::WHITE_KING;
                pieces[Colors::WHITE][PieceTypes::KING].set_bit(sq, true);
                break;
            case 'k':
                squares[sq] = Pieces::BLACK_KING;
                pieces[Colors::BLACK][PieceTypes::KING].set_bit(sq, true);
                break;
            }
            if (pos == '/')
                sq -= 16;
            else
                sq += 1;
        }

        //  color set
        current_color = (color == "w");

        // castling
        castling_rights = 0b0000;
        for (auto &ch : castling)
        {
            if (ch == 'K')
                castling_rights |= 0b0001;
            else if (ch == 'Q')
                castling_rights |= 0b0010;
            else if (ch == 'k')
                castling_rights |= 0b0100;
            else if (ch == 'q')
                castling_rights |= 0b1000;
        }

        // en passant square
        en_passant_square = Squares::NO_SQUARE;
        if (enpassant.size() > 1)
            en_passant_square = Square(enpassant[1] - '1', enpassant[0] - 'a');

        // half move clock
        if (!halfmove_clock.empty())
            half_moves.push_back(std::stoi(halfmove_clock));

        // full move counter
        if (!fullmove_counter.empty())
            full_moves = std::stoi(fullmove_counter);

        // update land
        land = std::array<Bitboard, 2>{0ull, 0ull};
        for (PieceType p = PieceTypes::PAWN; p <= PieceTypes::KING; p++)
        {
            land[current_color] |= pieces[current_color][p];
            land[current_color.flip()] |= pieces[current_color.flip()][p];
        }

        cur_zobrist_hash = hash_calc();

        BoardState current_state{Pieces::NO_PIECE, castling_rights, en_passant_square, cur_zobrist_hash};
        board_state_array.push_back(current_state);
        checkers() = get_checkers();
        pinned_pieces() = get_pinned_pieces();
        accumulators.emplace_back();
        refresh_accumulators();
    };

    uint64_t hash_calc()
    {
        uint64_t hash = 0;
        if (!current_color)
        { // black
            hash ^= BBD::Zobrist::black_to_move;
        }

        // piece-square
        for (Square sq = Squares::A1; sq <= Squares::H8; sq++)
        {
            if (at(sq) != Pieces::NO_PIECE)
            {
                hash ^= BBD::Zobrist::piece_square_keys[64 * int(at(sq)) + sq];
            }
        }

        // castling
        // bit 0: WK, bit 1: WQ, bit 2: BK, bit 3: BQ
        // uint8_t castling_rights = get_castling_rights();
        if (0b0001 & castling_rights)
            hash ^= BBD::Zobrist::castling_keys[0];
        if ((1 << 1) & castling_rights)
            hash ^= BBD::Zobrist::castling_keys[1];
        if ((1 << 2) & castling_rights)
            hash ^= BBD::Zobrist::castling_keys[2];
        if ((1 << 3) & castling_rights)
            hash ^= BBD::Zobrist::castling_keys[3];

        // en_passant
        // Square en_passant = get_en_passant_square();
        if (en_passant_square != Squares::NO_SQUARE)
        {
            hash ^= BBD::Zobrist::en_passant_keys[en_passant_square];
        }

        return hash;
    }

    /// Updates the Board, assuming the move is legal
    /// \param move
    /// \return
    void make_move(const Move &move)
    {
        accumulators.push_back(accumulators.back());
        auto &accumulator = accumulators.back();

        // zobrist incremental update (part 1)
        uint64_t new_zobrsist_hash = cur_zobrist_hash;
        new_zobrsist_hash ^= BBD::Zobrist::black_to_move;
        if (en_passant_square != Squares::NO_SQUARE)
        {
            new_zobrsist_hash ^= BBD::Zobrist::en_passant_keys[en_passant_square];
        }

        // clear previous target en_passant
        en_passant_square = Squares::NO_SQUARE;

        Square from = move.from();
        Square to = move.to();
        Piece captured = Pieces::NO_PIECE;

        // castling 0x1111 - bit 0: WK, bit 1: WQ, bit 2: BK, bit 3: BQ
        // update castling when King moves
        if (squares[from].type() == PieceTypes::KING)
        {
            if (current_color == Colors::WHITE)
            {
                castling_rights &= 0b1100; // remove for white
            }
            else
            {
                castling_rights &= 0b0011; // remove for black
            }
        }
        // update castling when Rook moves
        if (squares[from].type() == PieceTypes::ROOK)
        {
            if (from == Squares::A1 && current_color == Colors::WHITE)
            { // White queen side
                castling_rights &= 0b1101;
            }
            else if (from == Squares::H1 && current_color == Colors::WHITE)
            { // White king side
                castling_rights &= 0b1110;
            }
            else if (from == Squares::A8 && current_color == Colors::BLACK)
            { // Black queen side
                castling_rights &= 0b0111;
            }
            else if (from == Squares::H8 && current_color == Colors::BLACK)
            { // Black king side
                castling_rights &= 0b1011;
            }
        }

        // update bitmap for captured piece
        if (squares[to] != Pieces::NO_PIECE)
        {
            pieces[current_color.flip()][squares[to].type()].set_bit(to, false);

            // zobrist incremental update (part 2)
            new_zobrsist_hash ^= BBD::Zobrist::piece_square_keys[64 * int(at(to)) + to];

            land[current_color.flip()].set_bit(to, false);
            captured = squares[to];
            accumulator[0].remove_feature(feature_index(squares[to], to, Colors::BLACK));
            accumulator[1].remove_feature(feature_index(squares[to], to, Colors::WHITE));
        }

        // update castling when Rook is captured
        if (captured.type() == PieceTypes::ROOK)
        {
            Color color_captured = captured.color();
            if (color_captured == Colors::WHITE)
            {
                if (to == Squares::A1)
                { // White queen side
                    castling_rights &= 0b1101;
                }
                else if (to == Squares::H1)
                { // White king side
                    castling_rights &= 0b1110;
                }
            }
            else if (color_captured == Colors::BLACK)
            {
                if (to == Squares::A8)
                { // Black queen side
                    castling_rights &= 0b0111;
                }
                else if (to == Squares::H8)
                { // Black king side
                    castling_rights &= 0b1011;
                }
            }
        }

        // zobrist incremental update (part 3)
        if (board_state_array.back().castling != castling_rights)
        {
            // previous casting rights
            if (0b0001 & board_state_array.back().castling)
                new_zobrsist_hash ^= BBD::Zobrist::castling_keys[0];
            if ((1 << 1) & board_state_array.back().castling)
                new_zobrsist_hash ^= BBD::Zobrist::castling_keys[1];
            if ((1 << 2) & board_state_array.back().castling)
                new_zobrsist_hash ^= BBD::Zobrist::castling_keys[2];
            if ((1 << 3) & board_state_array.back().castling)
                new_zobrsist_hash ^= BBD::Zobrist::castling_keys[3];

            // new castling rights
            if (0b0001 & castling_rights)
                new_zobrsist_hash ^= BBD::Zobrist::castling_keys[0];
            if ((1 << 1) & castling_rights)
                new_zobrsist_hash ^= BBD::Zobrist::castling_keys[1];
            if ((1 << 2) & castling_rights)
                new_zobrsist_hash ^= BBD::Zobrist::castling_keys[2];
            if ((1 << 3) & castling_rights)
                new_zobrsist_hash ^= BBD::Zobrist::castling_keys[3];
        }

        switch (move.type())
        {
        case CASTLE:
            pieces[current_color][PieceTypes::KING].set_bit(from, false);
            pieces[current_color][PieceTypes::KING].set_bit(to, true);

            // zobrist incremental update part 4
            new_zobrsist_hash ^= BBD::Zobrist::piece_square_keys[64 * int(at(from)) + from];
            new_zobrsist_hash ^= BBD::Zobrist::piece_square_keys[64 * int(at(from)) + to];

            accumulator[0].remove_feature(feature_index(at(from), from, Colors::BLACK));
            accumulator[1].remove_feature(feature_index(at(from), from, Colors::WHITE));
            accumulator[0].add_feature(feature_index(at(from), to, Colors::BLACK));
            accumulator[1].add_feature(feature_index(at(from), to, Colors::WHITE));

            land[current_color].set_bit(from, false);
            land[current_color].set_bit(to, true);

            std::swap(squares[from], squares[to]);
            // queen's side
            if (to < from)
            {
                from = player_color() == Colors::WHITE ? Squares::A1 : Squares::A8;
                to = player_color() == Colors::WHITE ? Squares::D1 : Squares::D8;
            }
            else
            {
                from = player_color() == Colors::WHITE ? Squares::H1 : Squares::H8;
                to = player_color() == Colors::WHITE ? Squares::F1 : Squares::F8;
            }
            break;

        case ENPASSANT: {
            auto to_pos = to + 8 - 16 * current_color;
            pieces[current_color.flip()][PieceTypes::PAWN].set_bit(to_pos, false);

            // zobrist incremental update part 5
            new_zobrsist_hash ^= BBD::Zobrist::piece_square_keys[64 * int(at(to_pos)) + to_pos];

            accumulator[0].remove_feature(feature_index(at(to_pos), to_pos, Colors::BLACK));
            accumulator[1].remove_feature(feature_index(at(to_pos), to_pos, Colors::WHITE));

            land[current_color.flip()].set_bit(to_pos, false);
            captured = squares[to_pos];
            squares[to_pos] = Pieces::NO_PIECE;
            break;
        }
        case PROMO_KNIGHT:
        case PROMO_BISHOP:
        case PROMO_ROOK:
        case PROMO_QUEEN:
            pieces[current_color][PieceTypes::PAWN].set_bit(from, false);

            // zobrist incremental update part 6
            new_zobrsist_hash ^= BBD::Zobrist::piece_square_keys[64 * int(at(from)) + from];

            accumulator[0].remove_feature(feature_index(at(from), from, Colors::BLACK));
            accumulator[1].remove_feature(feature_index(at(from), from, Colors::WHITE));

            land[current_color].set_bit(from, false);
            squares[from] = (current_color ? Piece(2 * move.promotion_piece() + 1) : Piece(2 * move.promotion_piece()));
            pieces[current_color][move.promotion_piece()].set_bit(to, true);

            // zobrist incremetal update part 7
            new_zobrsist_hash ^= BBD::Zobrist::piece_square_keys[64 * int(at(from)) + from];

            accumulator[0].add_feature(feature_index(at(from), from, Colors::BLACK));
            accumulator[1].add_feature(feature_index(at(from), from, Colors::WHITE));
            // new_zobrsist_hash ^= BBD::Zobrist::piece_square_keys[at(from) * 64 + to];

            land[current_color].set_bit(to, true);
            break;
        }

        // check for 2 square move
        if (squares[from].type() == PieceTypes::PAWN && std::abs((int)from - (int)to) == 16)
        {
            // board_state_array.back().en_passant = (to + from) / 2;
            en_passant_square = Square(((int)to + (int)from) / 2); // we update it for the next move
        }

        // update the bitmap for moving piece
        pieces[current_color][squares[from].type()].set_bit(from, false);
        pieces[current_color][squares[from].type()].set_bit(to, true);

        // zobrist incremetal update part 8
        new_zobrsist_hash ^= BBD::Zobrist::piece_square_keys[64 * int(at(from)) + from];
        new_zobrsist_hash ^= BBD::Zobrist::piece_square_keys[64 * int(at(from)) + to];

        land[current_color].set_bit(from, false);
        land[current_color].set_bit(to, true);

        if (squares[from].type() == PieceTypes::PAWN)
            half_moves.push_back(-1);

        accumulator[0].remove_feature(feature_index(squares[from], from, Colors::BLACK));
        accumulator[1].remove_feature(feature_index(squares[from], from, Colors::WHITE));
        accumulator[0].add_feature(feature_index(squares[from], to, Colors::BLACK));
        accumulator[1].add_feature(feature_index(squares[from], to, Colors::WHITE));

        // make move
        std::swap(squares[to], squares[from]);
        squares[from] = Pieces::NO_PIECE;

        half_moves.back()++;
        if (current_color == Colors::BLACK)
            full_moves++;

        current_color = current_color.flip();

        // zobrist incremetal update part 9
        if (en_passant_square != Squares::NO_SQUARE)
        {
            new_zobrsist_hash ^= BBD::Zobrist::en_passant_keys[en_passant_square];
        }

        // cur_zobrist_hash = hash_calc();
        cur_zobrist_hash = new_zobrsist_hash;
        // hash_cnt[cur_zobrist_hash]++;

        // record the board state before the move is made
        BoardState current_state{captured, castling_rights, en_passant_square, cur_zobrist_hash};
        // record the current state
        board_state_array.push_back(current_state);

        pinned_pieces() = get_pinned_pieces();
        checkers() = get_checkers();
    };

    void make_null_move()
    {
        accumulators.push_back(accumulators.back());
        en_passant_square = Squares::NO_SQUARE;

        // zobrist incremental update (part 1)
        uint64_t new_zobrsist_hash = cur_zobrist_hash;
        new_zobrsist_hash ^= BBD::Zobrist::black_to_move;
        if (en_passant_square != Squares::NO_SQUARE)
        {
            new_zobrsist_hash ^= BBD::Zobrist::en_passant_keys[en_passant_square];
        }

        half_moves.back()++;
        if (current_color == Colors::BLACK)
            full_moves++;

        current_color = current_color.flip();
        cur_zobrist_hash = new_zobrsist_hash;
        board_state_array.emplace_back(Pieces::NO_PIECE, castling_rights, en_passant_square, cur_zobrist_hash);
        pinned_pieces() = get_pinned_pieces();
        checkers() = get_checkers();
    }
    /// Updates the Board, assuming the move is legal
    /// \param move
    /// \return
    void undo_move(const Move &move)
    {
        accumulators.pop_back();
        Piece prev_captured = board_state_array.back().captured;
        board_state_array.pop_back();

        Square from = move.from();
        Square to = move.to();

        if (current_color == Colors::WHITE)
            full_moves--;
        half_moves.back()--;
        if (half_moves.back() == -1)
            half_moves.pop_back();

        current_color = current_color.flip();

        // previous state
        BoardState prev_state = board_state_array.back();
        en_passant_square = prev_state.en_passant;
        castling_rights = prev_state.castling;
        cur_zobrist_hash = prev_state.zobrist_hash;

        switch (move.type())
        {
        case CASTLE:
            pieces[current_color][PieceTypes::KING].set_bit(from, true);
            pieces[current_color][PieceTypes::KING].set_bit(to, false);
            std::swap(squares[from], squares[to]);

            land[current_color].set_bit(from, true);
            land[current_color].set_bit(to, false);
            // queen's side
            if (to < from)
            {
                from = player_color() == Colors::WHITE ? Squares::A1 : Squares::A8;
                to = player_color() == Colors::WHITE ? Squares::D1 : Squares::D8;
            }
            else
            {
                from = player_color() == Colors::WHITE ? Squares::H1 : Squares::H8;
                to = player_color() == Colors::WHITE ? Squares::F1 : Squares::F8;
            }
            break;
        case ENPASSANT: {
            auto to_pos = to + 8 - 16 * current_color;
            pieces[current_color.flip()][PieceTypes::PAWN].set_bit(to_pos, true);
            land[current_color.flip()].set_bit(to_pos, true);
            squares[to_pos] = prev_captured;
            break;
        }
        case PROMO_KNIGHT:
        case PROMO_BISHOP:
        case PROMO_ROOK:
        case PROMO_QUEEN:
            auto promotion_piece_type = move.promotion_piece();
            pieces[current_color][promotion_piece_type].set_bit(to, false);
            land[current_color].set_bit(to, false);
            squares[from] = prev_captured;
            if (prev_captured != Pieces::NO_PIECE)
            {
                pieces[current_color.flip()][prev_captured.type()].set_bit(to, true);
                land[current_color.flip()].set_bit(to, true);
            }
            if (current_color == Colors::WHITE)
            {
                squares[to] = Pieces::WHITE_PAWN;
            }
            else
            {
                squares[to] = Pieces::BLACK_PAWN;
            }
            break;
        }
        pieces[current_color][squares[to].type()].set_bit(to, false);
        pieces[current_color][squares[to].type()].set_bit(from, true);

        land[current_color].set_bit(to, false);
        land[current_color].set_bit(from, true);

        if (move.type() == ENPASSANT)
        {
            squares[from] = squares[to];
            squares[to] = Pieces::NO_PIECE;
        }
        else
        {
            // undo move
            std::swap(squares[to], squares[from]);

            squares[to] = prev_captured;
            if (squares[to] != Pieces::NO_PIECE)
            {
                pieces[current_color.flip()][squares[to].type()].set_bit(to, true);
                land[current_color.flip()].set_bit(to, true);
            }
        }
    };

    /// Updates the Board for null move
    /// \return
    void undo_null_move()
    {
        accumulators.pop_back();
        board_state_array.pop_back();

        if (current_color == Colors::WHITE)
            full_moves--;
        half_moves.back()--;

        current_color = current_color.flip();

        BoardState prev_state = board_state_array.back();
        en_passant_square = prev_state.en_passant;
        castling_rights = prev_state.castling;
        cur_zobrist_hash = prev_state.zobrist_hash;
    }

    bool threefold_check()
    {
        int cnt = 0;
        for (auto it : board_state_array)
        {
            cnt += (it.zobrist_hash == cur_zobrist_hash);
            if (cnt == 3)
            {
                return true;
            }
        }
        return false;
    }

    const Bitboard get_pinned_pieces() const;

    const Bitboard get_checkers() const;

    const Bitboard get_pawn_attacks(const Color &color) const
    {
        const Bitboard pawns = pieces[color][PieceTypes::PAWN];
        const int file_a = color == Colors::WHITE ? 0 : 7, file_h = 7 - file_a;
        return (pawns & ~attacks::file_mask[file_a]).shift<NORTHWEST>(color) |
               (pawns & ~attacks::file_mask[file_h]).shift<NORTHEAST>(color);
    }

    template <int moves_type> int gen_legal_moves(MoveList &moves);

    /// Checks if the move is legal
    /// \param move
    /// \return
    bool is_legal(const Move &move) const;

    /// Returns the piece at square
    /// \param square
    /// \return
    Piece at(const Square &square) const
    {
        return squares[square];
    }

    const Bitboard orthogonal_sliders(const Color &color) const
    {
        return pieces[color][PieceTypes::ROOK] | pieces[color][PieceTypes::QUEEN];
    }

    const Bitboard diagonal_sliders(const Color &color) const
    {
        return pieces[color][PieceTypes::BISHOP] | pieces[color][PieceTypes::QUEEN];
    }

    /// Improve by keeping track of this incrementally
    const Bitboard all_pieces(const Color &color) const
    {
        return land[color];
    }

    /// Returns the color of the current player
    /// \return
    Color player_color() const
    {
        return current_color;
    }
    uint8_t halfmoves_clock() const
    {
        return half_moves.back();
    }
    uint8_t fullmoves_counter() const
    {
        return full_moves;
    }
    bool is_capture(const Move move) const
    {
        return move.type() == MoveTypes::ENPASSANT || move.is_promo() || at(move.to()) != Pieces::NO_PIECE;
    }

    // TODO: incrementally update
    void refresh_accumulators()
    {
        std::array<NNUE::NNUENetwork::Accumulator, 2> &accumulator = accumulators.back();
        accumulator = std::array<NNUE::NNUENetwork::Accumulator, 2>{};

        for (Square sq = 0; sq < 64; sq++)
        {
            Piece piece = squares[sq];
            if (piece)
            {
                accumulator[0].add_feature(feature_index(piece, sq, Colors::BLACK));
                accumulator[1].add_feature(feature_index(piece, sq, Colors::WHITE));
            }
        }
    }
    static int feature_index(Piece piece, Square square, Color perspective)
    {
        /*
         * maps feature to accumulator index (piece.type() + 6 * color()) * 64 + square
         * white_pawn = 0, white_knight = 1, ..., black_pawn = 6, ..., black_king = 11 (for white perspective)
         */
        return perspective == Colors::BLACK ? 64 * (piece.color() * 6 + piece.type()) + (square ^ 56)
                                            : 64 * ((piece.color().flip()) * 6 + piece.type()) + square;
    }
    std::array<NNUE::NNUENetwork::Accumulator, 2> &get_accumulators()
    {
        return accumulators.back();
    }

  private:
    std::array<Piece, 64> squares;
    std::array<std::array<Bitboard, 6>, 2> pieces;
    std::array<Bitboard, 2> land;
    Color current_color;
    uint8_t castling_rights;
    Square en_passant_square;
    uint64_t cur_zobrist_hash;
    std::vector<std::array<NNUE::NNUENetwork::Accumulator, 2>> accumulators;

    struct BoardState
    {
        Piece captured;
        uint8_t castling;
        Square en_passant;
        uint64_t zobrist_hash;
        Bitboard checkers, pinned_pieces;
        constexpr BoardState(Piece captured, uint8_t castling, Square en_passant, uint64_t zobrist_hash)
            : captured(captured), castling(castling), en_passant(en_passant), zobrist_hash(zobrist_hash)
        {
        }
    };

    std::vector<BoardState> board_state_array;

    std::vector<int> half_moves;
    uint8_t full_moves = 0;
};
} // namespace BBD
