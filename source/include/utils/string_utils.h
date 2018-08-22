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

//根据 key 作为分隔符，key 个数不限,只要是连在一块的，都算作分隔符
void split_by_key(const std::string& str, const char& key, std::vector<std::string>& result);
std::string to_lower(const std::string& str);

}
}
}

#endif
