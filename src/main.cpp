#include <board.h>
#include <bits/stdc++.h>
#include <utils.h>
#include <move.h>

int main(){

    Board board = Board();

    Move move1 = Move("e2e4");
    Move move2 = Move("d7d5");
    Move move3 = Move("e4e5");
    Move move4 = Move("f7f5");
    Move move5 = Move("e5f6");

    board.make_move(move1);
    board.make_move(move2);
    board.make_move(move3);
    board.make_move(move4);
    board.make_move(move5);


    board.print_board();


}