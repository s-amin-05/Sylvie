#include <logger.h>
#include <iostream>
#include <fstream>
#include <board.h>

Logger::Logger(std::string filename) {
    file_.open(filename, std::ios::out | std::ios::trunc);
    if (!file_.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

void Logger::log_to_file(std::string msg) {
    if (file_.is_open()) {
        file_ << msg << std::endl;
    } else {
        std::cerr << "Log file is not open." << std::endl;
    }
}

void Logger::log_board_to_file(Board &board) {
    if (file_.is_open()) {
        file_ << "Board State:" << std::endl;
    }
}


void Logger::close_file() {
    if (file_.is_open()) {
        file_.close();
    }
}

Logger::~Logger() {
    close_file();
}