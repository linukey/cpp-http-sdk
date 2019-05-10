/*
    author: linukey
    time: 2017.11.12
*/

#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

#include <iostream>
#include <string>
#include <fstream>

namespace linukey {
namespace webserver {
namespace utils {

void file_read_all(const std::string& filename, std::string& buffer);

}
}
}

#endif
