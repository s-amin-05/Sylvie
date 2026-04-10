#pragma once
#include <board.h>
#include <climits>
#include <unordered_map>

namespace Evaluation {
    int evaluate(const Board &board);
    int get_endgame_phase(const Board &board);
    int get_mopup_score(const Board &board,int color, int friend_material_score, int enemy_material_score, int endgame_phase);
    int get_piece_material_value(int piece_type);
    int get_position_bonus(int piece_type, int square, int color, int endgame_phase);
    int center_manhattan_distance(int sq);
    int manhattan_distance(int sq1, int sq2);
}
