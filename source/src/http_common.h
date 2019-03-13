#ifndef __LINUKEY_HTTP_COMMON_H__
#define __LINUKEY_HTTP_COMMON_H__

#include <iostream>
#include <unordered_map>

using std::string;
using std::unordered_map;

namespace linukey {
namespace webserver {
namespace http_common {

class HttpCommon {

public:

/*
 * post种类 : application/x-www-form-urlencoded
 * data : 请求体
 * ret  : 解析结果
 */
static bool post_extract(const string& data,
                         unordered_map<string, string>& ret);

/*
 * post种类 : multipart/form-data
 * content_type : content-type header
 * data : 请求体
 * ret  : 解析结果
 */
static bool post_extract(const string& content_type,
                         const string& data,
                         unordered_map<string, string>& ret);

};

}
}
}

#endif
