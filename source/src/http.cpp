/*
    author: linukey
    time: 2017.11.12
*/

#include "../include/http.h"

using namespace linukey::webserver::utils;

namespace linukey{  
namespace webserver{    
namespace http{

void extract_header(const string& headers, const string& key, string& value){
    string result = to_lower(headers);
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

    vector<string> strs;
    split_by_key(kv, ':', strs);
    value = strs[1];
}

void extract_request_line(const string& headers, unordered_map<string, string>& result){
    size_t pos = headers.find("\r\n");
    if (pos == string::npos) {
        return;
    }
    string req_line = headers.substr(0, pos);
    vector<string> strs;
    split_by_key(req_line, ' ', strs);
    result["method"] = strs[0];
    result["url"] = strs[1];
    result["protocol"] = strs[2];
}

// 解析请求头
void extract_request(const string& headers, shared_ptr<Request> req){
    unordered_map<string, string> req_line;
    extract_request_line(headers, req_line);

    req->method = req_line["method"];
    req->url = req_line["url"];
    req->protocol = req_line["protocol"];

    string key = REQUEST_HEADERS_STR[CONTENT_LENGTH];
    extract_header(headers, key, req->headers[key]);
}

// 解析请求体
void extract_datas(const string& req_datas, unordered_map<string, string>& datas){
    if (!req_datas.length()) return;
    // split data by '&'
    vector<string> v_data;
    split_by_key(req_datas, '&', v_data);
    // split data k-v by '='
    for (auto data : v_data){   
        size_t pos = data.find('=');
        if (pos == string::npos) {
            return;
        }
        string key = data.substr(0, pos);
        pos = data.find('=');
        if (pos == string::npos) {
            return;
        }
        string value = data.substr(pos+1);
        datas[key] = value;
    }
}

}
}
}
