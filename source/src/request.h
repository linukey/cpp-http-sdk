/*
    author: linukey
    time: 2017.11.12
*/

#ifndef __LINUKEY_WEBSERVER_REQUEST__
#define __LINUKEY_WEBSERVER_REQUEST__

#include "utils/string_utils.h"

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

// 请求头
enum REQUEST_HEADERS{
    CONTENT_LENGTH = 0,
    CONTENT_TYPE,
    HOST,
    REQUEST_HEADER_NUMS
};

static vector<string> REQUEST_HEADERS_STR = {
    "content-length",  
    "content-type",
    "host"
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

public:
    void extract_request(const string& request);

protected:
    void extract_header(const string& headers, const string& key, string& value);
    void extract_request_line(const string& headers, unordered_map<string, string>& result);

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
                            "Connection: close\r\n" \
                            "Access-Control-Allow-Origin: *\r\n" \
                            "\r\n";

}
}
}

#endif
