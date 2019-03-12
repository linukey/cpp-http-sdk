/*
    author: linukey
    time: 2017.11.12
*/

#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace linukey {
namespace webserver {
namespace utils {

std::vector<std::string> SplitString(const std::string& str, const std::string& key);
std::string LowerString(const std::string& str);
std::string Trim(std::string s);

}
}
}

#endif
