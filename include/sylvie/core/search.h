#pragma once
#include <board.h>
#include <atomic>
#include <thread>
#include <movegen.h>


class Searcher {
    Move best_move_;
    int best_evaluation_;
    u64 nodes_searched_;
    u64 quiescence_nodes_searched_;
    TranspositionTable table_;

    int minmax_search(int depth, Board &board);

    int alpha_beta_pruning(int depth, int alpha, int beta, Board &board, int ply);

    int alpha_beta_pruning_tt(int depth, int alpha, int beta, Board &board, int ply);

    int quiescence_search(int alpha, int beta, Board &board, int ply);

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
    u64 get_qs_nodes_searched() const;

    void resize_transposition_table(int size_mb);
    // void set_stop_search_flag(bool flag);


};