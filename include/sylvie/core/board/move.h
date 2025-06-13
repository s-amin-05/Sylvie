#pragma once
#include <string>
#include <piece.h>
#include <square.h>
#include <constants.h>

// First 3 bits for starting square 0-63
// Next 3 bits for target square 0-63
// Next 4 bits for promotion piece
// Next 4 bits for castling type
// Next bit for capture
// Last bit no use
class Move
{
public:
    u16 move_;
    Square starting_square_;
    Square target_square_;
    Piece promotion_piece_;
    u8 castling_type_;
    bool is_capture_;

    Move();

    Move(u16 move);

    Move(std::string move_notation);
};
