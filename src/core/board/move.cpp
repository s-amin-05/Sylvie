#include <constants.h>
#include <move.h>
#include <stdexcept>

Move::Move():
    starting_square_(chess::square::EMPTY),
    target_square_(chess::square::EMPTY),
    promotion_piece_(chess::piece::EMPTY),
    is_castling_(false),
    is_capture_(false)
{}


Move::Move(std::string move_notation) {
    if (move_notation.length() < 4 || move_notation.length() > 5) {
        throw std::invalid_argument("Invalid move notation length");
    }
    if (move_notation.length() == 5) {
        promotion_piece_ = Piece(move_notation[4]);
    }
    starting_square_ = Square(move_notation.substr(0, 2));
    target_square_ = Square(move_notation.substr(2, 2));

    is_castling_ = false;
    is_capture_ = false;
}

void Move::set_castling(bool castling) {
    is_castling_ = castling;
}

void Move::set_capture(bool capture) {
    is_capture_ = capture;
}

