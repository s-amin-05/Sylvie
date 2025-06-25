#include <board.h>
#include <move.h>
#include <string>


int main(){

    Board board = Board();
    //
    //
    Move move1 = Move("e2e4");
    Move move2 = Move("e7e5");
    Move move3 = Move("g1f3");
    Move move4 = Move("b8c6");
    Move move5 = Move("f1c4");
    Move move6 = Move("g8f6");
    Move move7 = Move("e1g1");
    //
    // Move move1 = Move("e2e4");
    // Move move2 = Move("d7d5");
    // Move move3 = Move("e4d5");
    //
    //
    board.make_move(move1);
    board.make_move(move2);
    board.make_move(move3);
    board.make_move(move4);
    board.make_move(move5);
    board.make_move(move6);
    board.make_move(move7);
    // board.unmake_move();
    // board.unmake_move();
    

}