#pragma once

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
    constexpr Board()
    {
        captured_pieces.reserve(300);
        half_moves.reserve(300);
        squares.fill(Pieces::NO_PIECE);

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
            if (!squares[sq].color())
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
    };

    Board(const std::string &fen)
    {
        captured_pieces.reserve(300);
        half_moves.reserve(300);
        squares.fill(Pieces::NO_PIECE);

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

        // en passant square

        // castling

        // half move clock
        half_moves.push_back(std::stoi(halfmove_clock));

        // full move counter
        full_moves = std::stoi(fullmove_counter);
    };

    /// Updates the Board, assuming the move is legal
    /// \param move
    /// \return
    bool make_move(const Move &move)
    {
        Square from = move.from();
        Square to = move.to();

        // update bitmap for captured piece
        if (squares[to] != Pieces::NO_PIECE)
        {
            pieces[current_color.flip()][squares[to].type()].set_bit(to, false);
            half_moves.push_back(-1);
        }
        captured_pieces.push_back(squares[to]);

        switch (move.type())
        {
        case CASTLE:
            // queen's side
            if (to > from)
            {
                pieces[current_color][PieceTypes::KING].set_bit(to + 1, false);
                pieces[current_color][PieceTypes::KING].set_bit(to - 1, true);
            }
            else
            {
                pieces[current_color][PieceTypes::KING].set_bit(to + 1, true);
                pieces[current_color][PieceTypes::KING].set_bit(to - 1, false);
            }
            std::swap(squares[to + 1], squares[to - 1]);
            break;

        case ENPASSANT: {
            auto to_pos = to + 8 - 16 * current_color;
            pieces[current_color.flip()][PieceTypes::PAWN].set_bit(to_pos, false);
            captured_pieces.push_back(squares[to_pos]);
            squares[to_pos] = Pieces::NO_PIECE;
            break;
        }
        case PROMO_KNIGHT:
        case PROMO_BISHOP:
        case PROMO_ROOK:
        case PROMO_QUEEN:
            pieces[current_color][PieceTypes::PAWN].set_bit(from, false);
            captured_pieces.push_back(squares[from]);
            squares[from] = (current_color ? Piece(2 * move.promotion_piece() + 1) : Piece(2 * move.promotion_piece()));
            pieces[current_color][move.promotion_piece()].set_bit(to, true);
            break;
        default:
            break;
        }

        // update the bitmap for moving piece
        pieces[current_color][squares[from].type()].set_bit(from, false);
        pieces[current_color][squares[from].type()].set_bit(to, true);

        if (squares[from].type() == PieceTypes::PAWN)
            half_moves.push_back(-1);

        // make move
        std::swap(squares[to], squares[from]);
        squares[from] = Pieces::NO_PIECE;

        half_moves.back()++;
        if (current_color == Colors::BLACK)
            full_moves++;

        current_color = current_color.flip();
        return true;
    };

    /// Updates the Board, assuming the move is legal
    /// \param move
    /// \return
    bool undo_move(const Move &move)
    {
        Square from = move.from();
        Square to = move.to();

        if (current_color == Colors::WHITE)
            full_moves--;
        half_moves.back()--;

        current_color = current_color.flip();

        switch (move.type())
        {
        case CASTLE:
            // queen's side
            if (to > from)
            {
                pieces[current_color][PieceTypes::KING].set_bit(to + 1, true);
                pieces[current_color][PieceTypes::KING].set_bit(to - 1, false);
            }
            else
            {
                pieces[current_color][PieceTypes::KING].set_bit(to + 1, false);
                pieces[current_color][PieceTypes::KING].set_bit(to - 1, true);
            }
            std::swap(squares[to + 1], squares[to - 1]);
            break;
        case ENPASSANT: {
            auto to_pos = to + 8 - 16 * current_color;
            pieces[current_color.flip()][PieceTypes::PAWN].set_bit(to_pos, true);
            squares[to_pos] = captured_pieces.back();
            captured_pieces.pop_back();
            break;
        }
        case PROMO_KNIGHT:
        case PROMO_BISHOP:
        case PROMO_ROOK:
        case PROMO_QUEEN:
            pieces[current_color][squares[to].type()].set_bit(to, true);
            squares[to] = captured_pieces.back();
            captured_pieces.pop_back();
            break;

        default:
            break;
        }
        pieces[current_color][squares[to].type()].set_bit(to, false);
        pieces[current_color][squares[to].type()].set_bit(from, true);

        // undo move
        std::swap(squares[to], squares[from]);
        squares[to] = captured_pieces.back();
        captured_pieces.pop_back();

        if (squares[from].type() == PieceTypes::PAWN)
            half_moves.pop_back();

        if (squares[to] != Pieces::NO_PIECE)
        {
            pieces[current_color.flip()][squares[to].type()].set_bit(to, true);
            half_moves.pop_back();
        }

        return true;
    };

    /// Returns the piece at square
    /// \param square
    /// \return
    Piece at(Square square) const
    {
        return squares[square];
    }

    /// Returns the color of the current player
    /// \return
    Color player_color() const
    {
        // TODO[geo_kuz]: Implement move validation
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

	~Board()
    {
        std::cerr << "deleting\n";
        captured_pieces.clear();
        half_moves.clear();
    };

  private:
    std::array<Piece, 64> squares;
    std::array<std::array<Bitboard, 6>, 2> pieces;
    Color current_color;
    std::vector<Piece> captured_pieces;

    std::vector<uint8_t> half_moves;
    uint8_t full_moves = 0;

    /// Checks if the move is legal
    /// \param move
    /// \return
    bool is_legal(const Move &move) const
    {
        return true;
    }
};
} // namespace BBD