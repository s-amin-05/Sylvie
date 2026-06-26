#pragma once
#include <constants.h>
#include <cctype>

class Piece {
public:
    // --- HOT PATH FUNCTIONS ---
    // Defined inline in the header so the compiler can embed the bitwise
    // operations directly into the move generator and evaluation loops.

    static inline int type_(const int piece) {
        return piece & bitmask::piece::PIECE_TYPE;
    }

    static inline int color_(const int piece) {
        return (piece & bitmask::piece::PIECE_COLOR) >> 3;
    }

    static inline int piece_(const int type, const int color) {
        return (color << 3) | type;
    }

    // --- COLD PATH FUNCTIONS ---
    // Used for I/O, FEN parsing, and UI. Definitions stay in the .cpp file.
    static char piece_notation(int piece);
    static int get_piece_from_notation(char piece_notation);
};