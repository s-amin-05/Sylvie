#pragma once
#include <constants.h>
#include <move.h>
#include <piece.h>
#include <square.h>
#include <stack>
#include <utils/logger.h>
using namespace std;


typedef struct IrreversibleState{
    u8 castling_rights;
    Square enpassant_target;
    int repetition_count;
    int halfmove_count;
    Piece captured_piece;
}IrreversibleState;

// Board class handles the complete state of the game
class Board{
public:
    string board_fen_;
    bool turn_;
    /*
     xxx1 - king side for black
     xx1x - queen side for black
     x1xx - king side for white
     1xxx - queen side for white
     */
    u8 castling_rights_;
    stack<Move> move_stack_;
    stack<IrreversibleState> irreversible_state_stack_;
    Square enpassant_target_;
    Piece board_[64];
    int ply_count_;
    int fullmove_number_;
    int repetition_count_;
    int halfmove_count_;
    Logger logger_;

    explicit Board(const string &position_fen);

    Board();

    void make_move(Move move);

    void unmake_move();

    void reset_board();

    // helper functions
    void setup_using_fen();

    void print_board();

};

