/*
    author: linukey
    time: 2017.11.12
*/

#include "request.h"

using namespace linukey::webserver::utils;

namespace linukey{  
namespace webserver{    
namespace request{

string Request::to_string() {
    string ret;
    ret = _method + SPACE + _url + SPACE + _protocol + CRLF;
    for (auto it = _headers.begin(); it != _headers.end(); ++it) {
        ret += it->first + ":" + it->second + CRLF;
    }
    ret += CRLF;
    ret += _data;
    return ret;
}

bool Request::post_extract(const string& data, unordered_map<string, string>& ret) {
    vector<string> lines = SplitString(data, "&");
    for (const auto& line : lines) {
        vector<string> kv = SplitString(line, "=");
        ret[kv[0]] = kv[1];
    }

    return true;
}

bool Request::post_extract(const string& content_type, const string& data, unordered_map<string, string>& ret) {
    vector<string> st = SplitString(content_type, "=");
    if (st.size() != 2) { return false; }

    string boundary = st[1];
    st = SplitString(data, boundary);

    // split by boundary
    for (const auto& i : st) {
        // split by /r/n/r/n
        vector<string> i_ret = SplitString(i, "\r\n\r\n");
        for (const auto& j : i_ret) {
            // split by \r\n
            vector<string> j_ret = SplitString(j, "\r\n");
            for (const auto& k : j_ret) {
                // split by ;
                vector<string> k_ret = SplitString(k, ";");
                for (const auto& m : k_ret) {
                    vector<string> r = SplitString(m, ":");
                    if (r.size() == 1) {
                        r = SplitString(m, "=");
                    }
                    if (r.size() != 1) {
                        string key = Trim(r[0]);
                        string val = Trim(r[1]);
                        
                        val.erase(0,val.find_first_not_of("\""));
                        val.erase(val.find_last_not_of("\"") + 1);

                        ret[key] = val;
                        if (key == "name") {
                            ret[val] = i_ret.back();
                        }
                    }
                }
            }
        }
    }

    return true;
}


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

    for (int i = CONTENT_LENGTH; i <HEADER_NUMS; ++i) {
        string key = HEADERS_STR[i];
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
