#include <square.h>
#include <constants.h>

int Square::rank_(const int square) {
    return square >> 3;
}

int Square::file_(const int square) {
    return square & 7;
}

int Square::square_(int file, int rank) {
    return rank << 3 | file;
}


int Square::get_square_from_notation(const std::string &square_notation) {
    if (square_notation == "00" || square_notation.size() != 2) {
        return chess::square::EMPTY;
    }

    int file = square_notation[0] - 'a';
    int rank = square_notation[1] - '1';

    return rank << 3 | file;

}

std::string Square::square_notation(int square) {
    if (square == chess::square::EMPTY) {
        return "-";
    }
    int file = file_(square);
    int rank = rank_(square);
    return std::string(1, 'a' + file) + std::to_string(rank + 1);
}

