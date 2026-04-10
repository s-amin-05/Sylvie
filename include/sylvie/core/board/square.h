#pragma once
#include <string>

class Square {
    public:
    static int file_(int square);
    static int rank_(int square);
    static int square_(int file, int rank);

    static int get_square_from_notation(const std::string &square_notation);
    static std::string square_notation(int square);

};