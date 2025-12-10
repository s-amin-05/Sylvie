#include <functional>
#include <uci.h>
#include <unordered_map>
#include <utils/utils.h>

namespace UCI {

    void handle_input(std::string &input) {
        input = Utils::trim(input);
        std::vector<std::string> input_tokens = Utils::split(input, ' ');
        std::string msg_type = input_tokens[0];
        std::vector<std::string> args(input_tokens.begin()+1, input_tokens.end());
        std::unordered_map<std::string, std::function<void(std::vector<std::string> &)>> uci_handler;


        uci_handler["uci"] = handle_uci;
        uci_handler["debug"] = handle_debug;
        uci_handler["isready"] = handle_isready;
        uci_handler["position"] = handle_position;
        uci_handler["go"] = handle_go;

        auto it = uci_handler.find(msg_type);
        if (it != uci_handler.end()) {
            it->second(args);
        }

    }

    void handle_uci(std::vector<std::string> &args) {

    }

    void handle_debug(std::vector<std::string> &args) {

    }

    void handle_isready(std::vector<std::string> &args) {

    }

    void handle_position(std::vector<std::string> &args) {

    }

    void handle_go(std::vector<std::string> &args) {

    }
}

