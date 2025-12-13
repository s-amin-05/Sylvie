#pragma once
#include <board.h>
#include <atomic>
#include <thread>


class Searcher {
    Move best_move_;
    int best_evaluation_;
    u64 nodes_searched_;

    int minmax_search(int depth, Board &board);

    int alpha_beta_pruning(int depth, int alpha, int beta, Board &board);

    public:
    Searcher();

    // temporarily added them here, will move
    // them to private and create wrappers around them
    std::atomic<bool> stop_search_;
    std::thread search_thread_;

    // updates the best move
    // depth needs to be passed for recursion
    void search_best_move(int depth, Board &board);

    Move get_best_move() const;
    int get_best_evaluation() const;

    u64 get_nodes_searched() const;

    // void set_stop_search_flag(bool flag);


};