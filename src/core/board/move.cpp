#include <constants.h>
#include <move.h>
#include <stdexcept>

Move::Move():
    move_(0),
    starting_square_(chess::square::EMPTY),
    target_square_(chess::square::EMPTY),
    promotion_piece_(chess::piece::EMPTY),
    castling_type_(0),
    is_capture_(false)
{}

Move::Move(u16 move) {
    move_ = move;
    starting_square_ = Square(static_cast<u8>(move & 0x07));
    starting_square_ = Square(static_cast<u8>((move >> 3U) & 0x07));
    promotion_piece_ = Piece(static_cast<u8>((move >> 7U) & 0x0F));
    castling_type_ = static_cast<u8>(move >> 11U) & 0x0F;
    is_capture_ = static_cast<u8>((move >> 15U) & 0x01);
}

Move::Move(std::string move_notation) {
    if (move_notation.length() < 4 || move_notation.length() > 5) {
        throw std::invalid_argument("Invalid move notation length");
    }
    if (move_notation.length() == 5) {
        promotion_piece_ = Piece(move_notation[4]);
    }
    starting_square_ = Square(move_notation.substr(0, 2));
    target_square_ = Square(move_notation.substr(2, 2));

    castling_type_ = 0;
    is_capture_ = false;
}

