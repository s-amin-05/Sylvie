#pragma once
#include <constants.h>
#include <move.h>
#include <stack>
#include <string>
#include <utils/logger.h>
#include <iostream>
#include <iomanip>
#include <cstdint>


struct IrreversibleState {
    Move move; // Storing the move here so we don't need a separate move_stack
    int castling_rights;
    int enpassant_target;
    int halfmove_count;
    int repetition_count;
    int captured_piece;
};
// piece-lists are used for generating bitboards later
// added count here for efficient removal of pieces

// Board class handles the complete state of the game
class Board{
public:
    std::string board_fen_;
    int turn_;
    /*
     xxx1 - king side for black
     xx1x - queen side for black
     x1xx - king side for white
     1xxx - queen side for white
     */
    IrreversibleState irreversible_history_[2048];
    Move move_history_[2048];
    int history_ply_ = 0;

    u8 castling_rights_;
    int enpassant_target_;
    int board_[64];
    int ply_count_;
    int fullmove_number_;
    int repetition_count_;
    int halfmove_count_;
    int captured_piece_;



    // just a temporary solution, for v1 only
    // will use bitboards and piece_lists for better consistency
    int white_king_square_;
    int black_king_square_;

    // for setting up endgame weight;
    std::vector<int> piece_count_;

    // piece-lists
    int piece_lists_[12][16];

    int piece_index_board_[64];

    // bitboards :)
    // ordering of bitboards same as piece lists
    u64 piece_bitboard_[12];
    u64 occupancy_white_;
    u64 occupancy_black_;

    // bitboard precomputed attack tables
    // pawns, knights and kings
    u64 pawn_attacks[2][64];
    u64 knight_attacks[64];
    u64 king_attacks[64];

    // // bitboards for squares attacked
    // u64 squares_attacked_piece_[12];
    // u64 squares_attacked_white_;
    // u64 squares_attacked_black_;

    Logger logger_;

    Board(const std::string &position_fen);

    Board();

    void make_move(Move &move, bool uci_flag);

    void unmake_move();

    void reset_board();

    // helper functions
    void setup_using_fen();

    void print_board() const;

    void print_bitboards() const;

};

