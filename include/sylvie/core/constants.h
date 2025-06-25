#pragma once
#include <cstdint>
#include <string>
using u8 = uint8_t; // Keeping typedef for backward compatibility
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// Constants are defined for later use
namespace chess{
    const std::string starting_pos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

namespace chess::piece{
    const int EMPTY = 0;
    const int KING = 1;
    const int QUEEN = 2;
    const int ROOK = 3;
    const int BISHOP = 4;
    const int KNIGHT = 5;
    const int PAWN = 6;
};

namespace chess::piecelists {
    const int WHITE_KING = 0;
    const int WHITE_QUEEN = 1;
    const int WHITE_ROOK = 2;
    const int WHITE_BISHOP = 3;
    const int WHITE_KNIGHT = 4;
    const int WHITE_PAWN = 5;
    const int BLACK_KING = 6;
    const int BLACK_QUEEN = 7;
    const int BLACK_ROOK = 8;
    const int BLACK_BISHOP = 9;
    const int BLACK_KNIGHT = 10;
    const int BLACK_PAWN = 11;
}

namespace chess::piecelists::max_count {
    const int ALL_PIECES = 32;
    const int KINGS = 1;
    const int QUEENS = 9;
    const int ROOKS = 10;
    const int BISHOPS = 10;
    const int KNIGHTS = 10;
    const int PAWNS = 8;
}

namespace chess::file {
    const int A = 0;
    const int B = 1;
    const int C = 2;
    const int D = 3;
    const int E = 4;
    const int F = 5;
    const int G = 6;
    const int H = 7;
}


namespace chess::square {
    const int EMPTY = 64;

    // First rank (1)
    const int A1 = 0;
    const int B1 = 1;
    const int C1 = 2;
    const int D1 = 3;
    const int E1 = 4;
    const int F1 = 5;
    const int G1 = 6;
    const int H1 = 7;

    // Second rank (2)
    const int A2 = 8;
    const int B2 = 9;
    const int C2 = 10;
    const int D2 = 11;
    const int E2 = 12;
    const int F2 = 13;
    const int G2 = 14;
    const int H2 = 15;

    // Third rank (3)
    const int A3 = 16;
    const int B3 = 17;
    const int C3 = 18;
    const int D3 = 19;
    const int E3 = 20;
    const int F3 = 21;
    const int G3 = 22;
    const int H3 = 23;

    // Fourth rank (4)
    const int A4 = 24;
    const int B4 = 25;
    const int C4 = 26;
    const int D4 = 27;
    const int E4 = 28;
    const int F4 = 29;
    const int G4 = 30;
    const int H4 = 31;

    // Fifth rank (5)
    const int A5 = 32;
    const int B5 = 33;
    const int C5 = 34;
    const int D5 = 35;
    const int E5 = 36;
    const int F5 = 37;
    const int G5 = 38;
    const int H5 = 39;

    // Sixth rank (6)
    const int A6 = 40;
    const int B6 = 41;
    const int C6 = 42;
    const int D6 = 43;
    const int E6 = 44;
    const int F6 = 45;
    const int G6 = 46;
    const int H6 = 47;

    // Seventh rank (7)
    const int A7 = 48;
    const int B7 = 49;
    const int C7 = 50;
    const int D7 = 51;
    const int E7 = 52;
    const int F7 = 53;
    const int G7 = 54;
    const int H7 = 55;

    // Eighth rank (8)
    const int A8 = 56;
    const int B8 = 57;
    const int C8 = 58;
    const int D8 = 59;
    const int E8 = 60;
    const int F8 = 61;
    const int G8 = 62;
    const int H8 = 63;
}

namespace chess::color{
    const bool WHITE = true;
    const bool BLACK = false;
};

namespace chess::move {
    const std::string NO_MOVE = "0000";
}

namespace movegen::sliding_offset {
    const int NORTH = 8;
    const int WEST = -1;
    const int SOUTH = -8;
    const int EAST = 1;
    const int NORTH_WEST = 7;
    const int SOUTH_WEST = -9;
    const int SOUTH_EAST = -7;
    const int NORTH_EAST = 9;
}

namespace movegen::knight_offset {
    const int NO_NO_WE = 15;
    const int NO_WE_WE = 6;
    const int SO_WE_WE = 10;
    const int SO_SO_WE = -17;
    const int SO_SO_EA = -15;
    const int SO_EA_EA = -6;
    const int NO_EA_EA = -10;
    const int NO_NO_EA = 17;
}

namespace chess::debug {
    const std::string line_seperator = "--------********--------\n";
}

