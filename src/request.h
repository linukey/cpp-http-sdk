/*
    author: linukey
    time: 2017.11.12
*/

#ifndef __LINUKEY_WEBSERVER_REQUEST__
#define __LINUKEY_WEBSERVER_REQUEST__

#include "http_common.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

using std::string;
using std::unordered_map;
using std::vector;
using std::shared_ptr;
using std::cout;
using std::endl;

using namespace linukey::webserver::http_common;

namespace linukey{  
namespace webserver{    
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
    static bool post_extract(const string& data,
                             unordered_map<string, string>& ret);

    /*
     * 功能 : 解析post请求体数据
     * post种类 : multipart/form-data
     * content_type : content-type header
     * data : 请求体
     * ret  : 解析结果
     */
    static bool post_extract(const string& content_type,
                             const string& data,
                             unordered_map<string, string>& ret);

    void extract_request(const string& request);

    /*
     * 功能 ：生成请求报文文本
     */
    string to_string();

protected:
    void extract_header(const string& headers, const string& key, string& value);
    void extract_request_line(const string& headers, unordered_map<string, string>& result);
    void parse_url();

public:
    void setMethod(const string& method);
    void setUrl(const string& url);
    void setProtocol(const string& protocol);
    void setHeader(const string& key, const string& val);
    void setData(const string& data);

    const string& getMethod() const;
    const string& getUrl() const;
    const string& getProtocol() const;
    const string& getHeader(const string& key);
    const string& getData() const;
    
    void printHeaders();
};

static const string HEADER = "HTTP/1.1 200 OK\r\n" \
                             "Content-Type: text/html\r\n" \
                             "Connection: close\r\n";
                            //"Access-Control-Allow-Origin: *\r\n";

}
}
}

#endif
