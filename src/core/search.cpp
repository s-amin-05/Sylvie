// #include <climits>
#include <eval.h>
#include <iostream>
#include <movegen.h>
#include <search.h>

constexpr int inf = 20000;

Searcher::Searcher() {
    best_move_ = Move();
    best_evaluation_ = 0;
}


int Searcher::minmax_search(int depth, Board &board) {
    if (stop_search_) return Evaluation::evaluate(board);

    if (depth == 0) {
        return Evaluation::evaluate(board);
    }

    auto move_generator = MoveGenerator();
    move_generator.generate_legal_moves(board);

    if (move_generator.legal_moves_.empty()) {
        if (move_generator.is_in_check(board, board.turn_)) {
            return -inf;
        }
        return 0;
    }

    int max_evaluation = -inf;

    for (Move move: move_generator.legal_moves_) {
        board.make_move(move);

        int evaluation = -minmax_search(depth-1, board);
        max_evaluation = std::max(evaluation, max_evaluation);

        board.unmake_move();

    }
    return max_evaluation;
}

void Searcher::search_best_move(int depth, Board &board) {
    auto move_generator = MoveGenerator();
    move_generator.generate_legal_moves(board);

    best_evaluation_ = -inf;
    best_move_ = move_generator.legal_moves_[0];

    for (auto move: move_generator.legal_moves_) {

        board.make_move(move);

        int move_eval = -minmax_search(depth-1, board);

        board.unmake_move();

        if (move_eval > best_evaluation_) {
            best_evaluation_ = move_eval;
            best_move_ = move;
        }
    }
}

Move Searcher::get_best_move() {
    return best_move_;
}

int Searcher::get_best_evaluation() {
    return best_evaluation_;
}

// void Searcher::set_stop_search_flag(bool flag) {
//     stop_search_ = flag;
// }