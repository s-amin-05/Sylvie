#include <constants.h>
#include <move.h>
#include <stdexcept>
#include <board.h>

Move::Move():
    starting_square_(chess::square::EMPTY),
    target_square_(chess::square::EMPTY),
    promotion_piece_(chess::piece::EMPTY),
    is_castling_(false),
    is_capture_(false),
    is_en_passant_(false)
{}

Move::Move(const int starting_square, const int target_square, const int promotion_piece,
           const bool is_castling, const bool is_capture, const bool is_en_passant):
    starting_square_(starting_square),
    target_square_(target_square),
    promotion_piece_(promotion_piece),
    is_castling_(is_castling),
    is_capture_(is_capture),
    is_en_passant_(is_en_passant)
{}

Move::Move(const std::string &move_notation): Move() {
    if (move_notation.length() < 4 || move_notation.length() > 5) {
        throw std::invalid_argument("Invalid move notation length");
    }


    starting_square_ = Square::get_square_from_notation(move_notation.substr(0, 2));
    target_square_ = Square::get_square_from_notation(move_notation.substr(2, 2));

    // promotion
    if (move_notation.length() == 5) {
        if (Square::rank_(target_square_) == 7) {
            promotion_piece_ = Piece::get_piece_from_notation(std::toupper(move_notation[4]));
        }else {
            promotion_piece_ = Piece::get_piece_from_notation(move_notation[4]);
        }
    }
}

std::string Move::get_move_notation() const {
    if (starting_square_ == chess::square::EMPTY && target_square_ == chess::square::EMPTY) {
        return chess::move::NO_MOVE;
    }
    std::string move_notation = Square::square_notation(starting_square_) + Square::square_notation(target_square_);
    if (Piece::type_(promotion_piece_) != chess::piece::EMPTY) {
        move_notation += std::tolower(Piece::piece_notation(promotion_piece_));
    }
    return move_notation;
}


