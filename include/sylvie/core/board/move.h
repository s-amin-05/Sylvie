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
    Square starting_square_;
    Square target_square_;
    Piece promotion_piece_;
    bool is_castling_;
    bool is_capture_;

    Move();

    Move(std::string move_notation);

    void set_castling(bool castling);

    void set_capture(bool capture);
};
