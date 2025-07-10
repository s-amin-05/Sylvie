#include <board.h>
#include <iostream>
#include <move.h>
#include <string>

#include <movegen.h>

#include "utils.h"


int main(){

    std::string fen = "r1bqkb1r/pppp1ppp/2n2n2/4p3/2b1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 1";
    Board board = Board(fen);

    Move move1 = Move("e2e4");
    Move move2 = Move("e7e5");
    Move move3 = Move("g1f3");
    Move move4 = Move("b8c6");
    Move move5 = Move("f1c4");
    Move move6 = Move("g8f6");
    Move move7 = Move("e1g1");

    // Move move1 = Move("e2e4");
    // Move move2 = Move("d7d5");
    // Move move3 = Move("e4e5");
    // Move move4 = Move("f7f5");

    // board.make_move(move1);
    // board.make_move(move2);
    // board.make_move(move3);
    // board.make_move(move4);
    // board.make_move(move5);
    // board.make_move(move6);
    // board.make_move(move7);


    // board.unmake_move();
    // board.unmake_move();

    Square square = Square("e1");
    MoveGenerator generator = MoveGenerator();
    generator.generate_legal_moves(board);

    for (auto move : generator.legal_moves_) {
        std::cout << move.get_move_notation() << std::endl;
    }

}