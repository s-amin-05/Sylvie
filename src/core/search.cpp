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
    quiescence_nodes_searched_ = 0ULL;
    table_.allocate(128);
}



int Searcher::minmax_search(const int depth, Board &board) {
    // count nodes searched
    nodes_searched_++;

    if (stop_search_) return Evaluation::evaluate(board);

    if (depth == 0) {
        return Evaluation::evaluate(board);
    }

    MoveList list;
    auto move_generator = MoveGenerator();
    move_generator.generate_legal_moves(board, list);

    if (list.empty()) {
        if (move_generator.is_in_check(board, board.turn_)) {
            return -chess::evaluation::INF;
        }
        return 0;
    }

    int max_evaluation = -chess::evaluation::INF;

    for (Move move: list) {
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

    MoveList list;
    auto move_generator = MoveGenerator();
    move_generator.generate_legal_moves(board, list);


    if (list.empty()) {
        if (move_generator.is_in_check(board, board.turn_)) {
            return -chess::evaluation::INF + ply;
        }
        return 0;
    }

    SearchUtils::order_moves(list, board, Move());

    for (Move move: list) {
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

int Searcher::alpha_beta_pruning_tt(int depth, int alpha, int beta, Board &board, int ply) {
    // count nodes searched
    nodes_searched_++;

    if (stop_search_) return Evaluation::evaluate(board);

    if (depth == 0) {
        return quiescence_search(alpha, beta, board, ply);
    }

    // 1. TT PROBE
    int tt_score = 0;
    Move tt_move; // Assuming 0 is an empty/null move

    // Assuming tt_ is a member variable of Searcher
    if (table_.probe(board.hash_, depth, alpha, beta, tt_score, tt_move)) {
        // Adjust mate scores from absolute (table) to relative (current search)
        if (tt_score > chess::evaluation::INF - 100) tt_score -= ply;
        if (tt_score < -chess::evaluation::INF + 100) tt_score += ply;
        return tt_score;
    }

    MoveList list;
    auto move_generator = MoveGenerator();
    move_generator.generate_legal_moves(board, list);

    if (list.empty()) {
        if (move_generator.is_in_check(board, board.turn_)) {
            return -chess::evaluation::INF + ply;
        }
        return 0;
    }

    // 2. MOVE ORDERING (Pass the TT move)
    SearchUtils::order_moves(list, board, tt_move);

    uint8_t tt_flag = TT_ALPHA; // Default to fail-low (upper bound)
    Move best_move;

    for (Move move: list) {
        board.make_move(move, false);
        int evaluation = -alpha_beta_pruning_tt(depth - 1, -beta, -alpha, board, ply + 1);
        board.unmake_move();

        if (stop_search_) return 0; // Prevent garbage stores if time runs out mid-search

        // 3. BETA CUTOFF (Fail-High / Lower Bound)
        if (evaluation >= beta) {
            int store_score = beta;
            // Adjust mate scores from relative (current search) to absolute (table)
            if (store_score > chess::evaluation::INF - 100) store_score += ply;
            if (store_score < -chess::evaluation::INF + 100) store_score -= ply;

            // Assuming your Move class can be converted to an integer
            table_.store(board.hash_, depth, store_score, TT_BETA, move);
            return beta;
        }

        // 4. FOUND A BETTER MOVE (Exact Bound)
        if (evaluation > alpha) {
            alpha = evaluation;
            tt_flag = TT_EXACT;
            best_move = move;
        }
    }

    // 5. STORE EXACT OR ALPHA
    int store_score = alpha;
    // Adjust mate scores before storing
    if (store_score > chess::evaluation::INF - 100) store_score += ply;
    if (store_score < -chess::evaluation::INF + 100) store_score -= ply;

    table_.store(board.hash_, depth, store_score, tt_flag, best_move);

    return alpha;
}

int Searcher::quiescence_search(int alpha, int beta, Board &board, int ply) {
    // count nodes (optional, usually tracked separately from main search)
    quiescence_nodes_searched_++;

    if (stop_search_) return Evaluation::evaluate(board);

    // 1. The "Stand Pat" lower bound
    int stand_pat = Evaluation::evaluate(board);

    // 2. Beta cutoff Check
    if (stand_pat >= beta) {
        return beta;
    }

    // 3. Update Alpha
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    MoveList list;
    auto move_generator = MoveGenerator();

    // 4. Generate ONLY captures
    move_generator.generate_captures(board, list);

    // 5. Order the captures
    SearchUtils::order_moves(list, board, Move()); // 0 = no TT move for basic QS

    // 6. The Search Loop
    for (Move move : list) {

        board.make_move(move, false);

        int score = -quiescence_search(-beta, -alpha, board, ply + 1);

        board.unmake_move();

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}

void Searcher::search_best_move(const int depth, Board &board) {

    nodes_searched_ = 0;
    quiescence_nodes_searched_ = 0;

    // Let the TT search handle everything, including the root!
    best_evaluation_ = alpha_beta_pruning_tt(depth, -chess::evaluation::INF, chess::evaluation::INF, board, 0);

    // We can retrieve the best move directly from the TT for the root position
    int tt_score;
    Move tt_move;
    if (table_.probe(board.hash_, depth, -chess::evaluation::INF, chess::evaluation::INF, tt_score, tt_move)) {
        best_move_ = tt_move;
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

u64 Searcher::get_qs_nodes_searched() const {
    return quiescence_nodes_searched_;
}

void Searcher::resize_transposition_table(int size_mb) {
    table_.allocate(size_mb);
}

// void Searcher::set_stop_search_flag(bool flag) {
//     stop_search_ = flag;
// }