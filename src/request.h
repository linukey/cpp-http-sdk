/*
    author: linukey
    time: 2017.11.12
*/

#ifndef __HTTP_HTTPSERVER_REQUEST__
#define __HTTP_HTTPSERVER_REQUEST__

#include "http_common.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>

#include "third/json.hpp"

using std::string;
using std::unordered_map;
using std::vector;
using std::shared_ptr;
using std::cout;
using std::endl;

using namespace http::common;
using Json = nlohmann::json;

namespace http{    
namespace request{

// 请求方法
enum REQUEST_METHOD{
    GET = 0,
    POST
};

static vector<string> REQUEST_METHOD_STR = {
    "get",
    "post"  
};

// 请求报文
class Request{
private:
    // 请求行
    string _method;   
    string _url;
    string _protocol;

    // 请求头
    unordered_map<string, string> _headers;

    // 请求体
    string _data;

// 对外提供工具类 API 接口
public:
    /*
     * 功能 : 解析post请求体数据
     * post种类 : application/x-www-form-urlencoded
     * data : 请求体
     * ret  : 解析结果
     */
    static bool post_extract_get(const string& data,
                             unordered_map<string, string>& ret);

    /*
     * 功能 : 解析post请求体数据
     * post种类 : json
     * data : 请求体
     * ret  : 解析结果
     */
    static bool post_extract_json(const string& data,
                                  std::map<string, string>& ret);

    /*
     * 功能 : 解析post请求体数据
     * post种类 : multipart/form-data
     * content_type : content-type header
     * data : 请求体
     * ret  : 解析结果
     */
    static bool post_extract_multipart(const string& content_type,
                                       const string& data,
                                       unordered_map<string, string>& ret);

    /*
     * 解析请求报文
     */
    void extract_request(const string& request_message);

    /*
     * 功能 ：生成请求报文文本
     */
    string to_string() const;

protected:
    /*
     * 从请求报文中解析headers
     */
    void extract_header(const string& request_message, const string& key, string& value);
    /*
     * 从请求报文中解析请求行
     */
    void extract_request_line(const string& request_message, unordered_map<string, string>& result);
    /*
     * 后处理url，去掉 协议后、host、参数、转小写
     */
    void parse_url();

public:
    void setMethod(const string& method);
    void setUrl(const string& url);
    void setProtocol(const string& protocol);
    void setHeader(const string& key, const string& val);
    void setData(const string& data);
    string& setData();

    const string& Method() const;
    const string& Url() const;
    const string& Protocol() const;
    const string& Header(const string& key);
    const string& Data() const;
    
    void printHeaders();
};

}}

#endif
