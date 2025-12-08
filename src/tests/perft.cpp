#include <chrono>
#include <constants.h>
#include <iostream>
#include <movegen.h>

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
        if (!generator.is_in_check(board))
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

void run_perft_tests() {
    std::vector<PerftTest> tests = {
        // {
        //     "Initial Position",
        //     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        //     {0,
        //      20,
        //      400,
        //      8902,
        //      197281,
        //      4865609,
        //      119060324}
        // },
        {
            "Kiwipete",
            "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
            {0,
             48,
             2039,
             97862,
             4085603,
             193690690}
        },
        {
            "Position 3",
            "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
            {0,
             14,
             191,
             2812,
             43238,
             674624,
             11030083}
        },
        {
            "Position 4",
            "r3k2r/p1pp1pb1/bn2Qnp1/3PN3/1p2P3/2N2q1p/PPPBBPPP/R3K2R b KQkq - 1 1",
            {0,
             6,
             264,
             9467,
             422333,
             15833292}
        }
    };


    for (auto &t : tests) {
        std::cout << "\n=== Running PERFT for: " << t.name << " ===\n";
        Board board(t.fen);

        for (int depth = 1; depth <= (int)t.expected.size() - 1; depth++) {
            auto start = std::chrono::high_resolution_clock::now();
            u64 nodes = perft(depth, board);
            auto end = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(end - start).count();

            std::cout << "Depth " << depth << ": " << nodes;

            if (depth < t.expected.size() && t.expected[depth] != 0) {
                if (nodes == t.expected[depth])
                    std::cout << "  [OK]";
                else
                    std::cout << "  [MISMATCH — expected " << t.expected[depth] << "]";
            }

            std::cout << "   (" << (u64)(nodes / elapsed) << " NPS)\n";
        }
        std::cout << std::endl;
    }
}



int main() {
    {
        // std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        // Board board = Board(fen);
        // perft_divide(1, board);
        // time_perft(5, board);
        run_perft_tests();

    }

    return 0;
}
