#include <board.h>
#include <iostream>
#include <string>
#include <uci.h>


int main(){
    auto uci_parser = UCI();

    std::string line;

    while (std::getline(std::cin, line)) {
        uci_parser.handle_input(line);
    }
}