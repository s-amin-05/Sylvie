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


Move::Move(std::string move_notation): Move() {
    if (move_notation.length() < 4 || move_notation.length() > 5) {
        throw std::invalid_argument("Invalid move notation length");
    }


    starting_square_ = Square(move_notation.substr(0, 2));
    target_square_ = Square(move_notation.substr(2, 2));

    // promotion
    if (move_notation.length() == 5) {
        if (target_square_.rank_ == 7) {
            promotion_piece_ = Piece(std::toupper(move_notation[4]));
        }else {
            promotion_piece_ = Piece(move_notation[4]);
        }
    }
}

std::string Move::get_move_notation() const {
    if (starting_square_.square_ == chess::square::EMPTY && target_square_.square_ == chess::square::EMPTY) {
        return chess::move::NO_MOVE;
    }
    std::string move_notation = starting_square_.get_square_notation() + target_square_.get_square_notation();
    if (promotion_piece_.piece_type_ != chess::piece::EMPTY) {
        move_notation += std::tolower(promotion_piece_.get_piece_notation());
    }
    return move_notation;
}


