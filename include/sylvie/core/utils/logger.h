#pragma once
#include <fstream>
#include <move.h>
#include <stack>
#include <string>



// forward declaration of board
class Board;
struct IrreversibleState;

class Logger {
    std::fstream file_;
    void close_file();

public:

    Logger(std::string filename);

    void log_to_file(std::string msg);

    void log_board_to_file(Board &board, Move move, bool detailed);

    // helper functions
    void log_board_state_to_file(Board &board);

    void log_irreversible_state_to_file(Board &board);

    void log_pieces_to_file(Piece board[]);

    void log_move_stack_to_file(std::stack<Move> &move_stack);

    void log_move_to_file(Move move);

    void log_irreversible_state_stack_to_file(std::stack<IrreversibleState> &irreversible_state_stack);

    ~Logger();
};
