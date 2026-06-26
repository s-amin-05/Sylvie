#pragma once
#include <string>

class Square {
    public:
    static int rank_(const int square) {
        return square >> 3;
    }

    static int file_(const int square) {
        return square & 7;
    }

    static int square_(int file, int rank) {
        return rank << 3 | file;
    }

    static int get_square_from_notation(const std::string &square_notation);
    static std::string square_notation(int square);

};