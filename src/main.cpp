#include <board.h>
#include <bits/stdc++.h>
#include <utils.h>
#include <move.h>

int main(){

    Board board = Board();

    Move move1 = MoveUtils::move_from_string("e2e4", board);
    board.make_move(move1);
    Move move2 = MoveUtils::move_from_string("d7d5", board);
    board.make_move(move2);
    Move move3 = MoveUtils::move_from_string("e4e5", board);
    board.make_move(move3);
    Move move4 = MoveUtils::move_from_string("f7f5", board);
    board.make_move(move4);
    Move move5 = MoveUtils::move_from_string("e5f6", board);
    board.make_move(move5);
    // Makemove ke time move ki value set karo

    board.print_board();


}