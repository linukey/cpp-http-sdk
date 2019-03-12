/*
    author: linukey
    time: 2017.11.12
*/

#include "string_utils.h"

using namespace std;

namespace linukey {
namespace webserver {
namespace utils {

vector<string> SplitString(const string& str, const string& key){
    vector<string> result;
    size_t pos = 0, pre_pos = 0;
    while((pos = str.find(key, pre_pos)) != string::npos){
        result.push_back(str.substr(pre_pos, pos-pre_pos));
        pre_pos = pos+key.size();
        if (pre_pos == str.length()) {
            result.push_back("");
        }
    }
    if (pre_pos < str.length() || str.size() == 0) {
        result.push_back(str.substr(pre_pos));
    }
    return result;
}

string LowerString(const string& str){
    string result;
    transform(str.begin(), str.end(), back_inserter(result), ::tolower); 
    return result;
}

// 不改变之前字符串
string Trim(string s) {
    if (s.empty()) { return s; }
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

}
}
}
