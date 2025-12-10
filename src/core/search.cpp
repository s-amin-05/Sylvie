// #pragma once
// #include <climits>
#include <movegen.h>
#include <search.h>
#include <eval.h>

namespace Search {
    void handle_best_evaluation(int &best_evaluation, int &evaluation, Move &best_move, Move &move) {
        if (evaluation > best_evaluation) {
            best_evaluation = evaluation;
            best_move = move;
        }
    }


    int minmax_search(int depth, Board &board, Move &best_move) {
        if (depth == 0) {
            return Evaluation::evaluate(board);
        }

        auto move_generator = MoveGenerator();
        move_generator.generate_legal_moves(board);
        if (move_generator.legal_moves_.empty()) {
            if (move_generator.is_in_check(board)) {
                return -INT_MAX;
            }
            return 0;
        }

        int best_evaluation = -INT_MAX;

        for (Move move: move_generator.legal_moves_) {
            board.make_move(move);

            int evaluation = -minmax_search(depth-1, board, best_move);
            handle_best_evaluation(best_evaluation, evaluation, best_move, move);

            board.unmake_move();
        }
        return best_evaluation;
    }
}