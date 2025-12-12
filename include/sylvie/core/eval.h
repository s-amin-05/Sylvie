#pragma once
#include <board.h>
#include <climits>
#include <unordered_map>

namespace Evaluation {
    int evaluate(Board &board);
    int count_material(Board &board, bool color);
    int get_position_bonus(int piece_type, int square, bool color);
}
