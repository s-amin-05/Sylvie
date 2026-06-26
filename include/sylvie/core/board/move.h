#pragma once
#include <piece.h>
#include <square.h>
#include <string>


// First 3 bits for starting square 0-63
// Next 3 bits for target square 0-63
// Next 4 bits for promotion piece
// Next 4 bits for castling type
// Next bit for capture
// Last bit no use
class Move
{
public:
    int starting_square_;
    int target_square_;
    int promotion_piece_;
    bool is_castling_;
    bool is_capture_;
    bool is_en_passant_;

    Move();

    Move(const std::string &move_notation);

    Move(int starting_square, int target_square, int promotion_piece, bool is_castling, bool is_capture, bool is_en_passant);

    std::string get_move_notation() const;
};
