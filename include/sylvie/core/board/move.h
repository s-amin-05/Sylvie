#pragma once
#include <piece.h>
#include <square.h>
#include <constants.h>


class Move
{
public:
    u16 move_;
    Square starting_square_;
    Square target_square_;
    Piece promotion_piece_;
    u8 castling_type_;
    bool is_capture;

    Move(u16 move);
};
