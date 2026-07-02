#pragma once
#include <board.h>
#include <vector>
#include <utils.h>

struct MoveList {
    Move moves[256]; // 256 is the absolute maximum number of legal moves in any chess position
    int count = 0;

    // Fast inline addition (no dynamic resizing, 0 heap allocations)
    void emplace_back(const Move& move) {
        moves[count++] = move;
    }

    // These allow you to still use range-based for loops (e.g., `for (Move m : move_list)`)
    Move* begin() { return &moves[0]; }
    Move* end() { return &moves[count]; }
    Move& operator[](size_t index) { return moves[index]; }
    const Move& operator[](size_t index) const { return moves[index]; }
    bool empty() const { return count == 0; }
    size_t size() const { return count; }
    void clear() { count = 0; }
};

class MoveGenerator {
    public:
    MoveGenerator();


    // // combination of all moves
    // std::vector<Move> pseudo_legal_moves_;
    // std::vector<Move> sliding_moves_;
    // std::vector<Move> pawn_moves_;
    // std::vector<Move> king_moves_;
    // std::vector<Move> knight_moves_;
    // std::vector<Move> nonking_moves;
    //
    // // legal moves are moves after which our king is not in check
    // std::vector<Move> legal_moves_;

    void generate_legal_moves(Board &board, MoveList &list);
    void generate_all_pseudo_legal_moves(Board &board, MoveList &list);

    void generate_sliding_piece_moves(const Board &board, MoveList &list, u64 check_mask, const u64 pin_mask[64]);
    void generate_pawn_moves(const Board &board, MoveList &list, u64 check_mask, const u64 pin_mask[64]);
    void generate_king_moves(const Board &board, int square, MoveList &list);
    void generate_knight_moves(const Board &board, MoveList &list, u64 check_mask, const u64 pin_mask[64]);

    // Quiescence search
    void generate_captures(Board &board, MoveList &list);
    void generate_sliding_piece_captures(const Board &board, MoveList &list, u64 check_mask, const u64 pin_mask[64]);
    void generate_knight_captures(const Board &board, MoveList &list, u64 check_mask, const u64 pin_mask[64]);
    void generate_king_captures(const Board &board, int square, MoveList &list);
    void generate_pawn_captures(const Board &board, MoveList &list, u64 check_mask, const u64 pin_mask[64]);


    bool is_in_check(const Board &board, int in_check_color);
};


