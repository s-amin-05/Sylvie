#pragma once
#include <constants.h>
#include <move.h>
#include <piece.h>
#include <square.h>
#include <stack>
#include <vector>
#include <string>
#include <utils/logger.h>


typedef struct IrreversibleState{
    u8 castling_rights;
    Square enpassant_target;
    int repetition_count;
    int halfmove_count;
    Piece captured_piece;
}IrreversibleState;

// piece-lists are used for generating bitboards later
// added count here for efficient removal of pieces

// Board class handles the complete state of the game
class Board{
public:
    std::string board_fen_;
    bool turn_;
    /*
     xxx1 - king side for black
     xx1x - queen side for black
     x1xx - king side for white
     1xxx - queen side for white
     */
    u8 castling_rights_;
    std::stack<Move> move_stack_;
    std::stack<IrreversibleState> irreversible_state_stack_;
    Square enpassant_target_;
    Piece board_[64];
    int ply_count_;
    int fullmove_number_;
    int repetition_count_;
    int halfmove_count_;


    Piece captured_piece_;

    // // bitboards :)
    // // ordering of bitboards same as piece lists
    // u64 piece_bb_[12];
    // u64 occupancy_white_;
    // u64 occupancy_black_;
    //
    // // bitboards for squares attacked
    // u64 squares_attacked_piece_[12];
    // u64 squares_attacked_white_;
    // u64 squares_attacked_black_;

    Logger logger_;

    Board(const std::string &position_fen);

    Board();

    void make_move(Move move);

    void unmake_move();

    void reset_board();

    // helper functions
    void setup_using_fen();

    void print_board();

};

