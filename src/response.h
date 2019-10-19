#ifndef __HTTP_HTTPSERVER_RESPONSE__
#define __HTTP_HTTPSERVER_RESPONSE__

#include "http_common.h"

#include <boost/algorithm/string.hpp>
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

using namespace http::common;

namespace http {
namespace response {    

class Response {
private:
    // 状态行
    string _protocol;
    string _status_code;
    string _status_describe;

    // 响应头
    unordered_map<string, string> _headers;

    // 响应体
    string _data;

public:
    /*
     * 生成响应报文文本
     */
    string to_string();

public:
    void setProtocol(const string& protocol);
    void setStatusCode(const string& status_code);
    void setStatusDescribe(const string& status_describe);
    void setHeader(const string& key, const string& val);
    void setData(const string& data);
    string& setData();

    const string& getProtocol() const;
    const string& getStatusCode() const;
    const string& getStatusDescribe() const;
    const string& getHeader(const string& key);
    const string& getData() const ;
    
};

}}

#endif
