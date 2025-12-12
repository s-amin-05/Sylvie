#pragma once
#include <atomic>
#include <board.h>

class Searcher {
    Move best_move_;
    int best_evaluation_;
    std::atomic<bool> stop_search_;

    int minmax_search(int depth, Board &board);

    public:
    Searcher();

    // updates the best move
    // depth needs to be passed for recursion
    void search_best_move(int depth, Board &board);
    Move get_best_move();
    int get_best_evaluation();

    void set_stop_search_flag(bool flag);


};