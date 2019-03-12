/*
    author: linukey
    time: 2017.11.12
*/

#include "request.h"

using namespace linukey::webserver::utils;

namespace linukey{  
namespace webserver{    
namespace request{

void extract_header(const string& headers, const string& key, string& value){
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

void extract_request_line(const string& headers, unordered_map<string, string>& result){
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
void extract_request(const string& request, shared_ptr<Request> req){
    unordered_map<string, string> req_line;
    extract_request_line(request, req_line);

    req->setMethod(req_line["method"]);
    req->setUrl(req_line["url"]);
    req->setProtocol(req_line["protocol"]);

    for (int i = CONTENT_LENGTH; i <REQUEST_HEADER_NUMS; ++i) {
        string key = REQUEST_HEADERS_STR[i];
        string val;
        extract_header(request, key, val);
        req->setHeader(key, val);
    }
}

}
}
}
