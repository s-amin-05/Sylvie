#pragma once
#include <fstream>
#include <string>

// forward declaration of board
class Board;

class Logger {
    std::fstream file_;
    void close_file();

public:

    Logger(std::string filename);

    void log_to_file(std::string msg);

    void log_board_to_file(Board &board);

    ~Logger();
};
