#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <board.h>
using u8=uint8_t;

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
// To be used later in the projet
// namespace BitUtils {
//     // Safe conversion to u8
//     constexpr u8 to_u8(auto value) {
//         return static_cast<u8>(static_cast<unsigned>(value) & 0xFFU);
//     }
//     // Safe bitwise OR
//     constexpr u8 bit_or(auto a, auto b) {
//         return to_u8(to_u8(a) | to_u8(b));
//     }
//     // Safe left shift
//     constexpr u8 shift_left(auto value, unsigned shift) {
//         return to_u8(to_u8(value) << shift);
//     }
//     // Safe right shift
//     constexpr u8 shift_right(auto value, unsigned shift) {
//         return to_u8(to_u8(value) >> shift);
//     }
//     // Safe bitwise AND
//     constexpr u8 bit_and(auto a, auto b) {
//         return to_u8(to_u8(a) & to_u8(b));
//     }
//     // Safe bitwise XOR
//     constexpr u8 bit_xor(auto a, auto b) {
//         return to_u8(to_u8(a) ^ to_u8(b));
//     }
// }

namespace  SearchUtils {
    void order_moves(std::vector<Move> &move_list, const Board &board);
}

namespace BitboardUtils {
    void print_single_bitboard(u64 bb, const std::string &name);


    void compute_attack_tables(Board &board);
    u64 compute_pawn_attacks(int square, int color);
    // void update_bitboard_from_move(Board &board, Move &move);
    void add_piece_to_bb(Board &board, int piece, int square);
    void remove_piece_from_bb(Board &board, int piece, int square);
    void update_piece_in_bb(Board &board, int piece, int starting_square, int target_square);

}
