#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <board.h>
using u8=uint8_t;

// forward ref
struct MoveList;

namespace Utils {
    std::vector<std::string> split(const std::string& s, char delim);
    std::string trim(const std::string& s);
    std::string get_fen_from_args(std::vector<std::string> &args);
    std::vector<std::string> get_moves_from_args(std::vector<std::string> &args);
    std::string get_arg_after_keyword(std::vector<std::string> &args, std::string keyword);
    bool is_digit(char c);
}

namespace MoveUtils {
    Move set_move_flags(Move &move, const Board &board);
    bool is_double_pawn_push(const Move &move, const Board &board);
}

namespace PieceListUtils {
    int get_piece_list_type(int piece);
    void add_piece_to_piece_list(int piece, int square, int piece_lists[][16], int piece_index_board[], std::vector<int> &piece_counts);
    void remove_piece_from_piece_list(int piece, int square, int piece_lists[][16], int piece_index_board[], std::vector<int> &piece_counts);
    void update_piece_list(int piece, int starting_square, int target_square, int piece_lists[][16], int piece_index_board[]);
}

namespace PieceCountUtils {
    void increment_piece_count(Board &board, int piece, int increment);
}

namespace BoardUtils {
    bool is_square_attacked(const Board &board, int square, int attacking_color);
}


namespace SearchUtils {
    int score_mvv_lva(const Move& move, const Board& board);
    void order_moves(MoveList &move_list, const Board &board, Move tt_move);
}

namespace BitboardUtils {
    void print_single_bitboard(u64 bb, const std::string &name);


    void compute_attack_tables(Board &board);
    u64 compute_pawn_attacks(int square, int color);
    u64 compute_knight_attacks(int square);
    u64 compute_king_attacks(int square);

    void compute_bishop_magic_bitboards(Board &board);
    void compute_rook_magic_bitboards(Board &board);

    u64 mask_bishop_attacks(int square);
    u64 raycast_bishop_attacks(int square, u64 blockers);
    u64 get_bishop_attacks(const Board &board, int square);
    u64 get_bishop_attacks(const Board &board, int square, u64 occupancy);


    u64 mask_rook_attacks(int square);
    u64 raycast_rook_attacks(int square, u64 blockers);
    u64 get_rook_attacks(const Board &board, int square);
    u64 get_rook_attacks(const Board &board, int square, u64 occupancy);

    void init_between_squares(Board &board);

    // void update_bitboard_from_move(Board &board, Move &move);
    void add_piece_to_bb(Board &board, int piece, int square);
    void remove_piece_from_bb(Board &board, int piece, int square);
    void update_piece_in_bb(Board &board, int piece, int starting_square, int target_square);

}

namespace ZobristUtils {

    void init_zobrist(Board &board);
    void generate_hash(Board& board);
}

enum TTFlag : u8 {
    TT_EXACT = 0, // Exact evaluation (principal variation node)
    TT_ALPHA = 1, // Upper bound (fail-low node)
    TT_BETA  = 2  // Lower bound (fail-high node)
};

// Align to 32 bytes for optimal memory access
struct alignas(32) TTEntry {
    Move best_move;
    uint64_t key;       // The full 64-bit Zobrist hash to handle collisions
    int16_t  score;     // The evaluation score
    uint8_t  depth;     // Depth to which this position was searched
    uint8_t  flag;      // TT_EXACT, TT_ALPHA, or TT_BETA
};

class TranspositionTable {
private:
    std::vector<TTEntry> table;
    size_t num_entries;

public:
    // Size in Megabytes (e.g., 64, 128, 256)
    void allocate(int size_mb);

    void clear();

    bool probe(uint64_t hash, int depth, int alpha, int beta, int& return_score, Move& return_move);

    void store(uint64_t hash, int depth, int score, int flag, Move best_move);
};

