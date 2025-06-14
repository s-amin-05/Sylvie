#pragma once
#include <cstdint>
#include <constants.h>

// First 3 bits for piece type
// Next bit for piece color
class Piece {
public:
    u8 piece_type_;
    bool piece_color_;

    Piece();

    Piece(char piece_notation);

    char get_piece_notation() const;
};
