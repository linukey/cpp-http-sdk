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
#include <algorithm>
#include <iterator>
#include <cctype>
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

// 请求报文
struct Request{
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
    void setMethod(const string& method) { _method = method; }
    void setUrl(const string& url) { _url = url; }
    void setProtocol(const string& protocol) { _protocol = protocol; }
    void setHeader(const string& key, const string& val) { _headers[key] = val; }
    void setData(const string& data) { _data = data; }

    const string& getMethod() const { return _method; }
    const string& getUrl() const { return _url; }
    const string& getProtocol() const { return _protocol; }
    const string& getHeader(const string& key) { return _headers[key]; }
    const string& getData() const { return _data; }
    
    void printHeaders() {
        for (auto it = _headers.begin(); it != _headers.end(); ++it) {
            cout << it->first << ":" << it->second << endl;
        }
    }
};

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

// help tools
void extract_request(const string& request, shared_ptr<Request> req);

static const string HEADER = "HTTP/1.1 200 OK\r\n" \
                            "Connection: close\r\n" \
                            "Access-Control-Allow-Origin: *\r\n" \
                            "\r\n";

}
}
}

#endif
