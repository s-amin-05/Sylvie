#include <chrono>
#include <constants.h>
#include <iostream>
#include <movegen.h>

#include <bot.h>
#include "search.h"
#define ANSI_RESET  "\033[0m"
#define ANSI_RED    "\033[31m"
#define ANSI_GREEN  "\033[32m"
#define ANSI_BOLD   "\033[1m"

u64 perft(int depth, Board &board) {
    u64 nodes = 0ULL;

    if (depth == 0)
        return 1ULL;


    MoveGenerator generator = MoveGenerator();
    generator.generate_legal_moves(board);
    // std::vector<Move> moves = generator.legal_moves_;

    for (auto move: generator.legal_moves_) {
        // std::cout << "Making... " <<move.get_move_notation() << std::endl;
        board.make_move(move);
        nodes += perft(depth - 1, board);
        // std::cout << "Unmaking... " <<move.get_move_notation() << std::endl;
        board.unmake_move();
    }
    return nodes;
}

u64 perft_pseudo(int depth, Board &board) {
    u64 nodes = 0ULL;

    if (depth == 0)
        return 1ULL;


    MoveGenerator generator = MoveGenerator();
    generator.generate_all_pseudo_legal_moves(board);
    // std::vector<Move> moves = generator.legal_moves_;

    for (auto move: generator.pseudo_legal_moves_) {
        // std::cout << "Making... " <<move.get_move_notation() << std::endl;
        board.make_move(move);
        if (!generator.is_in_check(board, !board.turn_))
            nodes += perft(depth - 1, board);
        // std::cout << "Unmaking... " <<move.get_move_notation() << std::endl;
        board.unmake_move();
    }
    return nodes;
}
void perft_divide(int depth, Board &board) {
    u64 total_nodes = 0ULL;

    MoveGenerator generator = MoveGenerator();
    generator.generate_legal_moves(board);
    // std::vector<Move> moves = generator.legal_moves_;

    for (auto move: generator.legal_moves_) {
        // board.make_move(move);
        board.make_move(move);
        u64 nodes = perft(depth - 1, board);
        board.unmake_move();

        std::cout << move.get_move_notation() << ": " << nodes << "\n";
        total_nodes += nodes;
    }
    std::cout << "Total Nodes: " << total_nodes << std::endl;
}

void time_perft(int depth, Board &board) {
    using namespace std::chrono;

    auto start = high_resolution_clock::now();
    u64 nodes = perft(depth, board);
    auto end = high_resolution_clock::now();

    duration<double> elapsed = end - start;

    std::cout << "Perft(" << depth << ") = " << nodes << " nodes\n";
    std::cout << "Time taken: " << elapsed.count() << " seconds\n";
    std::cout << "Nodes per second: " << static_cast<u64>(nodes / elapsed.count()) << " nps\n";
}

struct PerftTest {
    std::string name;
    std::string fen;
    std::vector<u64> expected; // expected[depth] = nodes
};

std::vector<PerftTest> tests = {
    {
        "Position 1 (Initial Position)",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        {0, 20, 400, 8902, 197281, 4865609, 119060324}
    },
    {
        "Position 2 (Kiwipete)",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        {0, 48, 2039, 97862, 4085603, 193690690}
    },
    {
        "Position 3",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
        {0, 14, 191, 2812, 43238, 674624, 11030083}
    },
    {
        "Position 4",
        "r3k2r/p1pp1pb1/bn2Qnp1/3PN3/1p2P3/2N2q1p/PPPBBPPP/R3K2R b KQkq - 1 1",
        {0, 4, 188, 8731, 372338, 16825938}
    },
    {
        "Position 5",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
        {0, 44, 1486, 62379, 2103487, 89941194}
    },
    {
        "Position 6",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
        {0, 46, 2079, 89890, 3894594, 164075551}
    }

};

void run_perft_test_suite() {


    for (auto &t : tests) {
        // I added ANSI_BOLD here to make the header pop a bit more
        std::cout << "\n" << ANSI_BOLD << "=== Running PERFT for: " << t.name << " ===" << ANSI_RESET << "\n";
        Board board(t.fen);

        for (int depth = 1; depth <= (int)t.expected.size() - 1; depth++) {
            auto start = std::chrono::high_resolution_clock::now();
            u64 nodes = perft(depth, board);
            auto end = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(end - start).count();

            // Calculate NPS (avoiding division by zero if it's too fast)
            u64 nps = (elapsed > 0) ? (u64)(nodes / elapsed) : 0;

            std::cout << "Depth " << depth << ": " << nodes;

            if (depth < t.expected.size() && t.expected[depth] != 0) {
                if (nodes == t.expected[depth]) {
                    // GREEN for Success
                    std::cout << ANSI_GREEN << "  [OK]" << ANSI_RESET;
                } else {
                    // RED for Failure
                    std::cout << ANSI_RED << "  [MISMATCH — expected " << t.expected[depth] << "]" << ANSI_RESET;
                }
            }

            std::cout << "   (" << nps << " NPS)\n";
        }
        std::cout << std::endl;
    }
}

void run_search_benchmark(const std::string& test_name, const std::string& fen, int depth) {
    std::cout << "--- " << test_name << " (Depth " << depth << ") ---" << std::endl;

    // 1. Setup
    Board board = Board(fen);
    Searcher searcher = Searcher();

    // 2. Start Timer
    auto start_time = std::chrono::high_resolution_clock::now();

    // 3. Run Search
    searcher.search_best_move(depth, board);

    // 4. Stop Timer
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    // 5. Output Results (Requested Format)
    std::cout << "BestMove: " << searcher.get_best_move().get_move_notation() << std::endl;
    std::cout << "Evaluation: " << searcher.get_best_evaluation() << std::endl;
    std::cout << "Nodes Searched: " << searcher.get_nodes_searched() << std::endl;

    // 6. Output Time
    std::cout << "Time: " << duration_ms << " ms" << std::endl;
    std::cout << "NPS: " << (u64) (searcher.get_nodes_searched() * 1000ULL) / (u64)(duration_ms + 1) << std::endl; // +1 to prevent div by zero
    std::cout << std::endl;
}

void run_search_test_suite() {
    int search_depth = 5;

    for (const auto& test : tests) {
        run_search_benchmark(test.name, test.fen, search_depth);
    }
    // auto test = tests[3];
    // run_search_benchmark(test.name, "r3k2r/p1ppqpb1/Bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1", search_depth);

}

int main() {
    {

        run_search_test_suite();
        // run_perft_test_suite();
    }

    return 0;
}
