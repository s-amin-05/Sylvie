#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <board.h>
using u8=uint8_t;

namespace Utils {
    std::vector<std::string> split(const std::string& s, char delim);

    bool is_digit(char c);
}


namespace MoveUtils {
    // Move move_from_string(std::string move_string, Board &board);

    Move set_move_flags(Move &move, Board &board);

    bool is_double_pawn_push(Move &move, Board &board);
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

