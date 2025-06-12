#include <board.h>

Square::Square(u8 square) {
    square_ = square;
    file_ = square % 8;
    rank_ = square / 8;
}

Square::Square(string square_notation) {
    char file = square_notation[0];
    char rank = square_notation[1];
    file_ = file - 'a';
    rank_ = rank - '1';
    square_ = rank_ * 8 + file_;
}

Square::Square() : square_(0), file_(0), rank_(0) {}
