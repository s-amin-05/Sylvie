#include <board.h>

Square::Square(u8 square) {
    square_ = square;
    file_ = square % 8;
    rank_ = square / 8;
}

Square::Square(string square_notation) {
    if (square_notation.length() == 2) {
        char file = square_notation[0];
        char rank = square_notation[1];
        file_ = file - 'a';
        rank_ = rank - '1';
        square_ = rank_ * 8 + file_;
    }else {
        throw std::invalid_argument("Invalid square notation length");
    }
}

Square::Square() : square_(0), file_(0), rank_(0) {}
