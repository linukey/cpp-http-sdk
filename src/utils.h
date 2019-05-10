/*
    author: linukey
    time: 2017.11.12
*/

#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

#include <iostream>
#include <string>
#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

namespace linukey {
namespace webserver {
namespace utils {

/*
 * 读取文件全部内容
 */
void file_read_all(const std::string& filename, std::string& buffer);

/*
 * gzip 压缩
 */
std::string gzip_compress(const std::string& text);

/*
 * gzip 解压
 */
std::string gzip_decompress(const std::string& text);

/*
 * urldecode
 */
void urldecode(const std::string& encd, std::string& decd);

}
}
}

#endif
