#pragma once
#include <string>

namespace UCI {
    void handle_input(std::string &input);

    void handle_uci(std::vector<std::string> &args);

    void handle_debug(std::vector<std::string> &args);

    void handle_isready(std::vector<std::string> &args);

    void handle_position(std::vector<std::string> &args);

    void handle_go(std::vector<std::string> &args);
}
