#pragma once
#include <constants.h>

class Piece {
    public:
    static int type_(int piece);
    static int color_(int piece);
    static int piece_(int type, int color);
    static char piece_notation(int piece);
    static int get_piece_from_notation(char piece_notation);
};