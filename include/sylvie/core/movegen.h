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
    std::vector<Move> nonking_moves;

    // legal moves are moves after which our king is not in check
    std::vector<Move> legal_moves_;

    void generate_legal_moves(Board &board);

    // generates all moves on the board (pseudo moves)
    void generate_all_pseudo_legal_moves(Board &board);

    // call the function for the piece
    void generate_pseudo_moves(Board &board, int square);

    void generate_sliding_piece_moves(const Board &board, const int &square);

    void generate_pawn_moves(const Board &board, int &square);

    void generate_king_moves(const Board &board, int &square);

    void generate_knight_moves(const Board &board, int &square);

    bool is_in_check(const Board &board, int in_check_color);
};


