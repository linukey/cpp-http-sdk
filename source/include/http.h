/*
    author: linukey
    time: 2017.11.12
*/

#ifndef __LINUKEY_WEBSERVER_REQUEST__
#define __LINUKEY_WEBSERVER_REQUEST__

#include "../include/utils/string_utils.h"

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

namespace linukey{  
namespace webserver{    
namespace http{

struct Request{
    unordered_map<string, string> headers;
    unordered_map<string, string> datas;

    string host;
    string method;   
    string url;
    string protocol;
};

enum REQUEST_METHOD{
    GET = 0,
    POST
};

static vector<string> REQUEST_METHOD_STR = {
    "get",
    "post"  
};

enum REQUEST_HEADERS{
    CONTENT_LENGTH = 0,
};

static vector<string> REQUEST_HEADERS_STR = {
    "content-length",  
};

static const string HEADER = "HTTP/1.1 200 OK\r\n" \
                            "Connection: close\r\n" \
                            "Access-Control-Allow-Origin: *\r\n" \
                            "\r\n";

// help tools
void extract_header(const string& headers, const string& key, string& value);
void extract_datas(const string& req_datas, unordered_map<string, string>& datas);
void extract_request(const string& headers, shared_ptr<Request> req);
void extract_request_line(const string& headers, unordered_map<string, string>& req_line);

}
}
}

#endif
