#include "helpers.h"

std::vector<std::string> stringSplit(const std::string& str, const char* delim) {
    std::size_t previous = 0;
    std::size_t current = str.find_first_of(delim);
    std::vector<std::string> elems;
    while (current != std::string::npos) {
        if (current > previous) {
            elems.push_back(str.substr(previous, current - previous));
        }
        previous = current + 1;
        current = str.find_first_of(delim, previous);
    }
    if (previous != str.size()) {
        elems.push_back(str.substr(previous));
    }
    return elems;
}

std::string GetCellNo(int32_t row, int32_t col) {
    return GetCellCol(col) + std::to_string(row);
}

std::string GetCellCol(int32_t col) {
    std::string s;
    if (col == 0) {
        return "A";
    }

    while (col != 0){
        char c = col % 26;
        s = (char)(c + 'A') + s;
        col /=26;
    }
    return s;
}
