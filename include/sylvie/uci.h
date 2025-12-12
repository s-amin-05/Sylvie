#pragma once
#include <string>
#include<bot.h>

class UCI {
    Engine engine_;
    std::string input_;
    std::vector<std::string> input_tokens_;
    std::string command_type_;
    std::vector<std::string> args_;
public:
    UCI();

    void handle_input(std::string &input);

    void handle_uci();

    void handle_debug();

    void handle_isready();

    void handle_position();

    void handle_go();

    void handle_stop();

    void handle_quit();
};
