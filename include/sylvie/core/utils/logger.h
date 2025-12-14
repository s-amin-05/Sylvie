#pragma once
#include <fstream>
#include <move.h>
#include <stack>
#include <string>
#include <vector>


// forward declaration of board
class Board;
struct IrreversibleState;

class Logger {
    std::fstream file_;
    void close_file();

public:

    Logger(const std::string &filename);

    void log_to_file(const std::string &msg);

    void log_board_to_file(Board &board, const Move &move, bool detailed);

    // helper functions
    void log_board_state_to_file(const Board &board);

    void log_irreversible_state_to_file(const Board &board);

    void log_pieces_to_file(int board[]);

    void log_piece_list_to_file(std::vector<std::vector<Square>> &piece_lists, std::vector<int> &piece_counts);

    void log_piece_index_board_to_file(int *piece_index_board);

    void log_move_stack_to_file(std::stack<Move> &move_stack);

    void log_move_to_file(const Move &move);

    void log_irreversible_state_stack_to_file(std::stack<IrreversibleState> &irreversible_state_stack);

    ~Logger();
};
