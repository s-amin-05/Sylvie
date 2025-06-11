#include<bits/stdc++.h>
#include<movegen.h>
using namespace std;
using u64 = uint64_t;
using u8 = uint8_t;


class Piece{
public:
    u8 piece_;
    u8 piece_type_;
    bool piece_color_;

    Piece();

    Piece(u8 piece);
};


class Square{
public:
    u8 square_;
    u8 file_;
    u8 rank_;

    Square(u8 square);

    Square();
};

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

// Board class maintains the complete state of the game
class Board{
public:
    string board_fen_;
    bool turn_;
    u8 castling_rights_;
    stack<Move> move_stack_;
    Square enpassant_target_;
    Piece board_[64];
    int ply_count_;
    int repetition_count_;
    int draw_rule_count_50_;

    Board(string position_fen);

    void make_move(Move move);

    void unmake_move();

    void reset_board();

};

// Constants are defined for later use
namespace chess{
    string starting_pos_fen = "";
}

namespace chess::piece{
    u8 EMPTY = 0;
    u8 KING = 1;
    u8 QUEEN = 2;
    u8 ROOK = 3;
    u8 BISHOP = 4;
    u8 KNIGHT = 5;
    u8 PAWN = 6;
};

namespace chess::square{
    u8 EMPTY = 64;

    u8 A1 = 0;
    u8 B1 = 1;
    u8 C1 = 2;
    u8 D1 = 3;
    u8 E1 = 4;
    u8 F1 = 5;
    u8 G1 = 6;
    u8 H1 = 7;

    u8 A2 = 8;
    u8 B2 = 9;
    u8 C2 = 10;
    u8 D2 = 11;
    u8 E2 = 12;
    u8 F2 = 13;
    u8 G2 = 14;
    u8 H2 = 15;

    u8 A3 = 16;
    u8 B3 = 17;
    u8 C3 = 18;
    u8 D3 = 19;
    u8 E3 = 20;
    u8 F3 = 21;
    u8 G3 = 22;
    u8 H3 = 23;

    u8 A4 = 24;
    u8 B4 = 25;
    u8 C4 = 26;
    u8 D4 = 27;
    u8 E4 = 28;
    u8 F4 = 29;
    u8 G4 = 30;
    u8 H4 = 31;

    u8 A5 = 32;
    u8 B5 = 33;
    u8 C5 = 34;
    u8 D5 = 35;
    u8 E5 = 36;
    u8 F5 = 37;
    u8 G5 = 38;
    u8 H5 = 39;

    u8 A6 = 40;
    u8 B6 = 41;
    u8 C6 = 42;
    u8 D6 = 43;
    u8 E6 = 44;
    u8 F6 = 45;
    u8 G6 = 46;
    u8 H6 = 47;

    u8 A7 = 48;
    u8 B7 = 49;
    u8 C7 = 50;
    u8 D7 = 51;
    u8 E7 = 52;
    u8 F7 = 53;
    u8 G7 = 54;
    u8 H7 = 55;

    u8 A8 = 56;
    u8 B8 = 57;
    u8 C8 = 58;
    u8 D8 = 59;
    u8 E8 = 60;
    u8 F8 = 61;
    u8 G8 = 62;
    u8 H8 = 63;
   
}

namespace chess::color{
    bool WHITE = true;
    bool BLACK = false;
};