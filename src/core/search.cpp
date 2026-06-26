// #include <climits>
#include <eval.h>
#include <iostream>
#include <movegen.h>
#include <search.h>

#include "utils.h"


Searcher::Searcher() {
    best_move_ = Move();
    best_evaluation_ = 0;
    nodes_searched_ = 0ULL;
}



int Searcher::minmax_search(const int depth, Board &board) {
    // count nodes searched
    nodes_searched_++;

    if (stop_search_) return Evaluation::evaluate(board);

    if (depth == 0) {
        return Evaluation::evaluate(board);
    }

    auto move_generator = MoveGenerator();
    move_generator.generate_legal_moves(board);

    if (move_generator.legal_moves_.empty()) {
        if (move_generator.is_in_check(board, board.turn_)) {
            return -chess::evaluation::INF;
        }
        return 0;
    }

    int max_evaluation = -chess::evaluation::INF;

    for (Move move: move_generator.legal_moves_) {
        board.make_move(move, false);

        int evaluation = -minmax_search(depth-1, board);
        max_evaluation = std::max(evaluation, max_evaluation);

        board.unmake_move();

    }
    return max_evaluation;
}

int Searcher::alpha_beta_pruning(int depth, int alpha, int beta, Board &board, int ply) {
    // count nodes searched
    nodes_searched_++;

    if (stop_search_) return Evaluation::evaluate(board);

    if (depth == 0) {
        return Evaluation::evaluate(board);
    }

    auto move_generator = MoveGenerator();
    move_generator.generate_legal_moves(board);

    if (move_generator.legal_moves_.empty()) {
        if (move_generator.is_in_check(board, board.turn_)) {
            return -chess::evaluation::INF + ply;
        }
        return 0;
    }

    SearchUtils::order_moves(move_generator.legal_moves_, board);

    for (Move move: move_generator.legal_moves_) {
        board.make_move(move, false);
        int evaluation = -alpha_beta_pruning(depth-1, -beta, -alpha, board, ply + 1);
        board.unmake_move();

        if (evaluation >= beta) {
            return beta;
        }
        alpha = std::max(evaluation, alpha);

    }
    return alpha;
}

void Searcher::search_best_move(const int depth, Board &board) {
    auto move_generator = MoveGenerator();
    move_generator.generate_legal_moves(board);
    SearchUtils::order_moves(move_generator.legal_moves_, board);

    best_evaluation_ = -chess::evaluation::INF;
    best_move_ = move_generator.legal_moves_[0];
    nodes_searched_ = 0;

    for (auto move: move_generator.legal_moves_) {

        board.make_move(move, false);

        int move_eval = -alpha_beta_pruning(depth-1, -chess::evaluation::INF, chess::evaluation::INF, board, 1);

        board.unmake_move();

        if (move_eval > best_evaluation_) {
            best_evaluation_ = move_eval;
            best_move_ = move;
        }
    }
}

Move Searcher::get_best_move() const {
    return best_move_;
}

int Searcher::get_best_evaluation() const {
    return best_evaluation_;
}

u64 Searcher::get_nodes_searched() const {
    return nodes_searched_;
}

// void Searcher::set_stop_search_flag(bool flag) {
//     stop_search_ = flag;
// }