#pragma once
#include <cstdint>
#include <string>
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// Constants are defined for later use
namespace chess{
    const std::string starting_pos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

namespace chess::piece{
    const u8 EMPTY = 0;
    const u8 KING = 1;
    const u8 QUEEN = 2;
    const u8 ROOK = 3;
    const u8 BISHOP = 4;
    const u8 KNIGHT = 5;
    const u8 PAWN = 6;
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



namespace chess::square {
    const u8 EMPTY = 64;

    // First rank (1)
    const u8 A1 = 0;
    const u8 B1 = 1;
    const u8 C1 = 2;
    const u8 D1 = 3;
    const u8 E1 = 4;
    const u8 F1 = 5;
    const u8 G1 = 6;
    const u8 H1 = 7;

    // Second rank (2)
    const u8 A2 = 8;
    const u8 B2 = 9;
    const u8 C2 = 10;
    const u8 D2 = 11;
    const u8 E2 = 12;
    const u8 F2 = 13;
    const u8 G2 = 14;
    const u8 H2 = 15;

    // Third rank (3)
    const u8 A3 = 16;
    const u8 B3 = 17;
    const u8 C3 = 18;
    const u8 D3 = 19;
    const u8 E3 = 20;
    const u8 F3 = 21;
    const u8 G3 = 22;
    const u8 H3 = 23;

    // Fourth rank (4)
    const u8 A4 = 24;
    const u8 B4 = 25;
    const u8 C4 = 26;
    const u8 D4 = 27;
    const u8 E4 = 28;
    const u8 F4 = 29;
    const u8 G4 = 30;
    const u8 H4 = 31;

    // Fifth rank (5)
    const u8 A5 = 32;
    const u8 B5 = 33;
    const u8 C5 = 34;
    const u8 D5 = 35;
    const u8 E5 = 36;
    const u8 F5 = 37;
    const u8 G5 = 38;
    const u8 H5 = 39;

    // Sixth rank (6)
    const u8 A6 = 40;
    const u8 B6 = 41;
    const u8 C6 = 42;
    const u8 D6 = 43;
    const u8 E6 = 44;
    const u8 F6 = 45;
    const u8 G6 = 46;
    const u8 H6 = 47;

    // Seventh rank (7)
    const u8 A7 = 48;
    const u8 B7 = 49;
    const u8 C7 = 50;
    const u8 D7 = 51;
    const u8 E7 = 52;
    const u8 F7 = 53;
    const u8 G7 = 54;
    const u8 H7 = 55;

    // Eighth rank (8)
    const u8 A8 = 56;
    const u8 B8 = 57;
    const u8 C8 = 58;
    const u8 D8 = 59;
    const u8 E8 = 60;
    const u8 F8 = 61;
    const u8 G8 = 62;
    const u8 H8 = 63;
}

namespace chess::color{
    const bool WHITE = true;
    const bool BLACK = false;
};

namespace chess::move {
    const std::string NO_MOVE = "0000";
}

namespace movegen::sliding_directions {
    const u8 NORTH = 0;
    const u8 WEST = 1;
    const u8 SOUTH = 2;
    const u8 EAST = 3;
    const u8 NORTH_WEST = 4;
    const u8 SOUTH_WEST = 5;
    const u8 SOUTH_EAST = 6;
    const u8 NORTH_EAST = 7;
}

namespace chess::debug {
    const std::string line_seperator = "--------********--------\n";
}

