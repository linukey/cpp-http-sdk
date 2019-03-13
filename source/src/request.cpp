/*
    author: linukey
    time: 2017.11.12
*/

#include "request.h"

using namespace linukey::webserver::utils;

namespace linukey{  
namespace webserver{    
namespace request{

void Request::extract_header(const string& headers, const string& key, string& value){
    string result = LowerString(headers);
    size_t spos = result.find(key);
    if (spos == string::npos) {
        return;
    }

    size_t epos = result.find("\r\n", spos);
    if (epos == string::npos) {
        return;
    }

    string kv = result.substr(spos, epos-spos);

    size_t pos;
    while (kv.length() > 0 && (pos = kv.find(" ")) != string::npos) {
        kv.erase(pos, 1);    
    }

    vector<string> strs = SplitString(kv, ":");
    value = strs[1];
}

void Request::extract_request_line(const string& headers, unordered_map<string, string>& result){
    size_t pos = headers.find("\r\n");
    if (pos == string::npos) {
        return;
    }
    string req_line = headers.substr(0, pos);
    vector<string> strs = SplitString(req_line, " ");
    result["method"] = strs[0];
    result["url"] = strs[1];
    result["protocol"] = strs[2];
}

// 解析请求头
void Request::extract_request(const string& request){
    unordered_map<string, string> req_line;
    extract_request_line(request, req_line);

    _method = req_line["method"];
    _url = req_line["url"];
    _protocol = req_line["protocol"];

    for (int i = CONTENT_LENGTH; i <REQUEST_HEADER_NUMS; ++i) {
        string key = REQUEST_HEADERS_STR[i];
        string val;
        extract_header(request, key, val);
        _headers[key] =  val;
    }
}

void Request::setMethod(const string& method) { 
    _method = method;
}

void Request::setUrl(const string& url) { 
    _url = url; 
}

void Request::setProtocol(const string& protocol) { 
    _protocol = protocol; 
}

void Request::setHeader(const string& key, const string& val) { 
    _headers[key] = val; 
}

void Request::setData(const string& data) { 
    _data = data; 
}

const string& Request::getMethod() const { 
    return _method; 
}

const string& Request::getUrl() const { 
    return _url; 
}

const string& Request::getProtocol() const { 
    return _protocol; 
}

const string& Request::getHeader(const string& key) { 
    return _headers[key]; 
}

const string& Request::getData() const { 
    return _data; 
}

void Request::printHeaders() {
    for (auto it = _headers.begin(); it != _headers.end(); ++it) {
        cout << it->first << ":" << it->second << endl;
    }
}

}
}
}
