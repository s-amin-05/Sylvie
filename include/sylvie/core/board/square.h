#pragma once
#include <string>
#include <constants.h>


// 0-63 squares
class Square{
public:
    u8 square_;
    u8 file_;
    u8 rank_;

    Square();

    Square(u8 square);

    Square(std::string square_notation);

    std::string get_square_notation() const;
};