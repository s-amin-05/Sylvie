#pragma once
#include <board.h>
#include <vector>

class MoveGenerator {
    public:
    MoveGenerator();


    // combination of all moves
    std::vector<Move> pseudo_legal_moves_;
    std::vector<Move> sliding_moves_;
    std::vector<Move> pawn_moves_;
    std::vector<Move> king_moves_;
    std::vector<Move> knight_moves_;

    // legal moves are moves after which our king is not in check
    std::vector<Move> legal_moves_;

    // generates all moves on the board (pseudo moves)
    void generate_pseudo_moves(Board &board);

    void generate_sliding_piece_moves(Board &, Square &square);

    void generate_pawn_moves(Board &board, Square &square);

    void generate_king_moves(Board &board, Square &square);

    void generate_knight_moves(Board &board, Square &square);

    void generate_legal_moves(Board &board);
};


