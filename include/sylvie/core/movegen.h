#pragma once
#include <board.h>
#include <vector>

class MoveGenerator {
    public:
    MoveGenerator();

    // counterclockwise from North (first 4 - Rooks, next 4 - Bishops)
    const int _direction_offsets_[8] = {8, -1, -8, 1, 7, -9, -7, 9};

    // combination of all moves
    vector<Move> pseudo_legal_moves_;
    vector<Move> sliding_moves_;
    vector<Move> pawn_moves_;
    vector<Move> king_moves_;
    vector<Move> knight_moves_;

    // legal moves are moves after which our king is not in check
    vector<Move> legal_moves_;

    // generates all moves on the board
    void generate_moves(Board &board);

    void generate_sliding_piece_moves(Board &, Square &square);

    void generate_pawn_moves(Board &board);

    void generate_king_moves(Board &board);

    void generate_knight_moves(Board &board);

    void generate_legal_moves(Board &board);
};


