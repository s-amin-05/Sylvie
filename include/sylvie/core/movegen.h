#pragma once
#include <board.h>
#include <vector>

// First 6 bits - starting square
// Next 6 bits - target square
// Next 2 bits - castling side
// Next 1 bit - is the move a capture move
// Last bit - redundant
vector<Move> generate_moves(Board board);

vector<Move> generate_legal_moves(Board board);

// helper functions
Move get_move_from_string(string move_string, Board board);