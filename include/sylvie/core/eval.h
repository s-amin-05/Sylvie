#pragma once
#include <board.h>
#include <climits>
#include <unordered_map>

namespace Evaluation {
    int evaluate(const Board &board);
    int get_piece_material_value(int piece_type);
    int get_position_bonus(int piece_type, int square, int color);
}
