#ifndef _HELPERS_H__
#define _HELPERS_H__

#include <vector>
#include <string>

std::vector<std::string> stringSplit(const std::string& str, const char* delim);
std::string GetCellNo(int32_t row, int32_t col);

#endif
