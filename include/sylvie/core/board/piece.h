#pragma once
#include <cstdint>
#include <constants.h>

// First 3 bits for piece type
// Next bit for piece color
class Piece {
    u8 piece_;
    u8 piece_type_;
    bool piece_color_;
public:
    Piece();

    Piece(u8 piece);

    Piece(char piece_notation);

    char get_piece_notation() const;
};
