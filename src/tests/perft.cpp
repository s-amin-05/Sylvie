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
    u64 nodes = perft_pseudo(depth, board);
    auto end = high_resolution_clock::now();

    duration<double> elapsed = end - start;

    std::cout << "Perft(" << depth << ") = " << nodes << " nodes\n";
    std::cout << "Time taken: " << elapsed.count() << " seconds\n";
    std::cout << "Nodes per second: " << static_cast<u64>(nodes / elapsed.count()) << " nps\n";
}


int main() {
    {
        // std::string fen = "rnbqkb1r/pP2pppp/5n2/3p4/8/8/PPP1PPPP/RNBQKBNR w KQkq d6 0 1";
        Board board = Board();

        // board.make_move(Move("b1a3"));
        // board.make_move(Move("e7e6"));
        // board.make_move(Move("a1b1"));
        // board.make_move(Move("g8e7"));
        // board.make_move(Move("b1a1"));
        //
        // MoveGenerator generator = MoveGenerator();
        // generator.generate_legal_moves(board);
        // perft_divide(6, board);
        time_perft(5, board);

    }


    // Board board = Board();
    // board.make_move(Move("b1a3"));
    // board.make_move(Move("e7e6"));
    // board.make_move(Move("a1b1"));
    // board.make_move(Move("g8e7"));
    // board.make_move(Move("b1a1"));
    // // board.make_move(Move("d2e1"));
    // // board.unmake_move();
    //
    // MoveGenerator generator = MoveGenerator();
    // generator.generate_legal_moves(board);
    // for (auto move: generator.legal_moves_) {
    //     std::cout << move.get_move_notation() << std::endl;
    // }

    return 0;
}
