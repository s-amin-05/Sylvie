#include <board.h>

Square::Square(u8 square) {
    square_ = square;
    if (square == chess::square::EMPTY) {
        file_ = 8;
        rank_ = 8;
        return;
    }
    file_ = square % 8;
    rank_ = square / 8;
}

Square::Square(string square_notation) {
    if (square_notation == "00") {
        square_ = chess::square::EMPTY;
        file_ = 8;
        rank_ = 8;
        return;
    }
    if (square_notation.length() == 2) {
        char file = square_notation[0];
        char rank = square_notation[1];
        file_ = static_cast<u8>(file - 'a');
        rank_ = static_cast<u8>(rank - '1');
        square_ = rank_ * 8 + file_;
    }else {
        throw std::invalid_argument("Invalid square notation length");
    }
}

Square::Square() : square_(0), file_(0), rank_(0) {}

std::string Square::get_square_notation() const {
    if (square_ == chess::square::EMPTY) {
        return "-";
    }
    return std::string(1, 'a' + file_) + std::to_string(rank_ + 1);
}
