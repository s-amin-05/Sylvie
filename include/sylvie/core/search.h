#pragma once
#include <board.h>

namespace Search {
    // updates the best move
    int minmax_search(int depth, Board &board, Move &best_move);
}