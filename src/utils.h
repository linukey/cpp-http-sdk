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

namespace http {
namespace utils {

/*
 * 读取文件全部内容
 */
void file_read_all(const std::string& filename, std::string& buffer);

/*
 * gzip 压缩
 */
void gzip_compress(const std::string& text, std::string& out_text);

/*
 * gzip 解压
 */
void gzip_decompress(const std::string& text, std::string& out_text);

/*
 * urldecode
 */
void urldecode(const std::string& encd, std::string& decd);

/*
 * 获取 url extension
 */
std::string get_extension_from_url(const std::string& url);

}}

#endif
