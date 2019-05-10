/*
    author: linukey
    time: 2017.11.12
*/

#include "request.h"

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
    vector<string> lines;
    boost::split(lines, data, boost::is_any_of("&"));
    for (const auto& line : lines) {
        vector<string> kv;
        boost::split(kv, line, boost::is_any_of("="));
        ret[kv[0]] = kv[1];
    }

    return true;
}

bool Request::post_extract(const string& content_type, const string& data, unordered_map<string, string>& ret) {
    vector<string> st;
    boost::split(st, content_type, boost::is_any_of("="));
    if (st.size() != 2) { return false; }

    string boundary = st[1];
    boundary = "--" + boundary;
    st.clear();
    boost::split(st, data, boost::is_any_of(boundary));

    // split by boundary
    for (const auto& i : st) {
        // split by /r/n/r/n
        vector<string> i_ret;
        boost::split(i_ret, i, boost::is_any_of("\r\n\r\n"));
        //for (const auto& j : i_ret) {
        for (int l = 0; l < i_ret.size()-1; ++l) {
            const auto &j = i_ret[l];
            // split by \r\n
            vector<string> j_ret;
            boost::split(j_ret, j, boost::is_any_of("\r\n"));
            for (const auto& k : j_ret) {
                // split by ;
                vector<string> k_ret;
                boost::split(k_ret, k, boost::is_any_of(";"));
                for (const auto& m : k_ret) {
                    vector<string> r;
                    boost::split(r, m, boost::is_any_of(":"));
                    if (r.size() == 1) {
                        r.clear();
                        boost::split(r, m, boost::is_any_of("="));
                    }
                    if (r.size() != 1) {
                        string key = boost::trim_copy(r[0]);
                        string val = boost::trim_copy(r[1]);
                        
                        val.erase(0,val.find_first_not_of("\""));
                        val.erase(val.find_last_not_of("\"") + 1);

                        ret[key] = val;
                        if (key == "name") {
                            ret[val] = boost::trim_copy(i_ret.back());
                        }
                    }
                }
            }
        }
    }

    return true;
}


void Request::extract_header(const string& headers, const string& key, string& value){
    string result = boost::algorithm::to_lower_copy(headers);
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
    boost::split(strs, kv, boost::is_any_of(":"));
    value = strs[1];
}

void Request::extract_request_line(const string& headers, unordered_map<string, string>& result){
    size_t pos = headers.find("\r\n");
    if (pos == string::npos) {
        return;
    }
    string req_line = headers.substr(0, pos);
    vector<string> strs;
    boost::split(strs, req_line, boost::is_any_of(" "));
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

    parse_url();

    for (int i = CONTENT_LENGTH; i <HEADER_NUMS; ++i) {
        string key = HEADERS_STR[i];
        string val;
        extract_header(request, key, val);
        _headers[key] =  val;
    }
}

void Request::parse_url() {
    size_t pos = _url.find("http://");
    if (pos == 0) {
        _url = _url.substr(7);
    }
    pos = _url.find("https://");
    if (pos == 0) {
        _url = _url.substr(8);
    }

    pos = _url.find("/");
    _url = _url.substr(pos);

    pos = _url.find("?");
    if (pos != string::npos) {
        _data = _url.substr(pos+1);
        _url = _url.substr(0, pos);
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
    string tmp_key = boost::algorithm::to_lower_copy(key);
    _headers[tmp_key] = val; 
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
    string tmp_key = boost::algorithm::to_lower_copy(key);
    return _headers[tmp_key]; 
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
