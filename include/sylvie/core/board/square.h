#pragma once
#include <string>
#include <constants.h>


// 0-63 squares
class Square{
public:
    int square_;
    int file_;
    int rank_;

    Square();

    Square(int square);

    Square(int file, int rank);

    Square(std::string square_notation);

    std::string get_square_notation() const;
};