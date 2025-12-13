#include <functional>
#include <uci.h>
#include <unordered_map>
#include <utils/utils.h>
#include <iostream>

UCI::UCI() = default;


void UCI::handle_input(const std::string &input) {
    input_ = Utils::trim(input);
    input_tokens_ = Utils::split(input_, ' ');
    command_type_ = input_tokens_[0];
    args_ = std::vector(input_tokens_.begin()+1, input_tokens_.end());
    std::unordered_map<std::string, std::function<void()>> uci_handler;


    uci_handler["uci"] = [this](){handle_uci();};
    uci_handler["debug"] = [this](){handle_debug();};
    uci_handler["isready"] = [this](){handle_isready();};
    uci_handler["position"] = [this](){handle_position();};
    uci_handler["go"] = [this](){handle_go();};
    uci_handler["stop"] = [this](){handle_stop();};
    uci_handler["quit"] = [this](){handle_quit();};

    auto it = uci_handler.find(command_type_);
    if (it != uci_handler.end()) {
        it->second();
    }

}

void UCI::handle_uci() {
    engine_.print_engine_info();
    std::cout << "uciok" << std::endl;
}

void UCI::handle_debug() {
    if (!args_.empty() && !args_[0].empty()) {
        bool flag = args_[0] == "on" ? true : false;
        engine_.set_debug_flag(flag);
    }
}

void UCI::handle_isready() {
    std::cout << "readyok" << std::endl;
}

void UCI::handle_position() {
    if (args_.empty()) return;

    if (args_[0] == "startpos") {
        engine_.setup_board(chess::starting_pos_fen);
    }
    else if (args_[0] == "fen") {
        std::string fen_string = Utils::get_fen_from_args(args_);
        engine_.setup_board(fen_string);
    }
    else return;

    std::vector<std::string> moves_to_play = Utils::get_moves_from_args(args_);
    for (auto move_str: moves_to_play) {
        Move move = Move(move_str);
        engine_.make_move(move);
    }

}

void UCI::handle_go() {
    // if (args_.empty()) return;

    int btime, winc, binc;
    int wtime = btime = winc = binc = 0;


    if (input_.contains("wtime")) {
        wtime = std::stoi(Utils::get_arg_after_keyword(args_, "wtime"));
    }
    if (input_.contains("btime")) {
        btime = std::stoi(Utils::get_arg_after_keyword(args_, "btime"));
    }
    if (input_.contains("winc")) {
        winc = std::stoi(Utils::get_arg_after_keyword(args_, "winc"));
    }
    if (input_.contains("binc")) {
        binc = std::stoi(Utils::get_arg_after_keyword(args_, "binc"));
    }

    engine_.set_times(wtime, btime, winc, binc);

    // handle depth, etc later


    // start search
    engine_.start_search();

}

void UCI::handle_stop() {
    engine_.stop_search();
}


void UCI::handle_quit() {
    exit(0);
}


