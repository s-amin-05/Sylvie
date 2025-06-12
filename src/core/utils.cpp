#include<utils.h>
#include <sstream>

std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string token;
    std::vector<std::string> string_parts;
    while (std::getline(ss, token, delim)) {
        string_parts.push_back(token);
    }

    return string_parts;
}

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

