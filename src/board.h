#pragma once

#include "attacks.h"
#include "bitboard.h"
#include "color.h"
#include "move.h"
#include "piece.h"
#include "square.h"
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

    Board()
    {
        half_moves.reserve(300);
        squares.fill(Pieces::NO_PIECE);
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

        BoardState current_state{Pieces::NO_PIECE, castling_rights, en_passant_square};
        board_state_array.push_back(current_state);
        checkers() = get_checkers();
        pinned_pieces() = get_pinned_pieces();
    };

    Board(const std::string &fen)
    {
        set_fen(fen);
    }

    void set_fen(const std::string &fen)
    {
        half_moves.reserve(300);
        squares.fill(Pieces::NO_PIECE);
        pieces[Colors::WHITE].fill(Bitboard(0ull));
        pieces[Colors::BLACK].fill(Bitboard(0ull));

        std::string parsing_fen = std::string(fen);
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
        half_moves.push_back(std::stoi(halfmove_clock));

        // full move counter
        full_moves = std::stoi(fullmove_counter);

        // update land
        land = std::array<Bitboard, 2>{0ull, 0ull};
        for (PieceType p = PieceTypes::PAWN; p <= PieceTypes::KING; p++)
        {
            land[current_color] |= pieces[current_color][p];
            land[current_color.flip()] |= pieces[current_color.flip()][p];
        }

        BoardState current_state{Pieces::NO_PIECE, castling_rights, en_passant_square};
        board_state_array.push_back(current_state);
        checkers() = get_checkers();
        pinned_pieces() = get_pinned_pieces();
    };

    /// Updates the Board, assuming the move is legal
    /// \param move
    /// \return
    void make_move(const Move &move)
    {
        // record the board state before the move is made
        BoardState current_state{Pieces::NO_PIECE, castling_rights, en_passant_square};
        // record the current state
        board_state_array.push_back(current_state);
        // clear previous target en_passant
        en_passant_square = Squares::NO_SQUARE;

        Square from = move.from();
        Square to = move.to();

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
            land[current_color.flip()].set_bit(to, false);
            board_state_array.back().captured = squares[to];
        }

        // update castling when Rook is captured
        if (board_state_array.back().captured.type() == PieceTypes::ROOK)
        {
            Color color_captured = board_state_array.back().captured.color();
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

        switch (move.type())
        {
        case CASTLE:
            pieces[current_color][PieceTypes::KING].set_bit(from, false);
            pieces[current_color][PieceTypes::KING].set_bit(to, true);

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
            land[current_color.flip()].set_bit(to_pos, false);
            board_state_array.back().captured = squares[to_pos];
            squares[to_pos] = Pieces::NO_PIECE;
            break;
        }
        case PROMO_KNIGHT:
        case PROMO_BISHOP:
        case PROMO_ROOK:
        case PROMO_QUEEN:
            pieces[current_color][PieceTypes::PAWN].set_bit(from, false);
            land[current_color].set_bit(from, false);
            squares[from] = (current_color ? Piece(2 * move.promotion_piece() + 1) : Piece(2 * move.promotion_piece()));
            pieces[current_color][move.promotion_piece()].set_bit(to, true);
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

        land[current_color].set_bit(from, false);
        land[current_color].set_bit(to, true);

        if (squares[from].type() == PieceTypes::PAWN)
            half_moves.push_back(-1);

        // make move
        std::swap(squares[to], squares[from]);
        squares[from] = Pieces::NO_PIECE;

        half_moves.back()++;
        if (current_color == Colors::BLACK)
            full_moves++;

        current_color = current_color.flip();
        pinned_pieces() = get_pinned_pieces();
        checkers() = get_checkers();
    };

    /// Updates the Board, assuming the move is legal
    /// \param move
    /// \return
    void undo_move(const Move &move)
    {
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
        castling_rights = prev_state.castling;
        en_passant_square = prev_state.en_passant;
        Piece prev_captured = prev_state.captured;

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
            board_state_array.pop_back();
        }
        else
        {
            // undo move
            std::swap(squares[to], squares[from]);

            squares[to] = prev_captured;
            board_state_array.pop_back();
            if (squares[to] != Pieces::NO_PIECE)
            {
                pieces[current_color.flip()][squares[to].type()].set_bit(to, true);
                land[current_color.flip()].set_bit(to, true);
            }
        }
    };

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

  private:
    std::array<Piece, 64> squares;
    std::array<std::array<Bitboard, 6>, 2> pieces;
    std::array<Bitboard, 2> land;
    Color current_color;
    uint8_t castling_rights;
    Square en_passant_square;

    struct BoardState
    {
        Piece captured;
        uint8_t castling;
        Square en_passant;
        Bitboard checkers, pinned_pieces;
        constexpr BoardState(Piece captured, uint8_t castling, Square en_passant)
            : captured(captured), castling(castling), en_passant(en_passant)
        {
        }
    };

    std::vector<BoardState> board_state_array;

    std::vector<int> half_moves;
    uint8_t full_moves = 0;
};
} // namespace BBD
