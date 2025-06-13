#include <board.h>
#include <bits/stdc++.h>
#include <utils.h>
#include <move.h>

int main(){

    Board board = Board();
    Move move1 = Move("e2e4");
    Move move2 = Move("e7e5");
    Move move3 = Move("g1f3");
    Move move4 = Move("b8c6");
    board.make_move(move1);
    board.make_move(move2);
    board.make_move(move3);
    board.make_move(move4);
    board.print_board();


}