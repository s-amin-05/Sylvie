#include <board.h>
#include <fstream>
#include <iostream>
#include <logger.h>
#include <move.h>
#include <string>

#include "movegen.h"


int main(){

    std::string fen = "rnbqkbnr/pP4pp/8/4pp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 1";
    Board board = Board();

    Move move1 = Move("e2e4");
    Move move2 = Move("e7e5");
    Move move3 = Move("g1f3");
    Move move4 = Move("b8c6");
    Move move5 = Move("f1c4");
    Move move6 = Move("g8f6");
    Move move7 = Move("e1g1");

    board.make_move(move1);
    board.make_move(move2);
    board.make_move(move3);
    board.make_move(move4);
    board.make_move(move5);
    board.make_move(move6);
    // board.make_move(move7);
    // board.unmake_move();
    // board.make_move(move7);

    MoveGenerator generator = MoveGenerator();
    Square square = Square("c4");
    generator.generate_sliding_piece_moves(board, square);

    for (auto move : generator.pseudo_legal_moves_) {
        std::cout << move.get_move_notation() << std::endl;
    }

    // std::fstream fs("board.log", ios::app | ios::in);
    // fs << "yayyy" << endl;
    // fs.close();





}