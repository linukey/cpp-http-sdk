#include "http_common.h"
#include "utils/string_utils.h"
#include <vector>

using std::vector;

namespace linukey {
namespace webserver {
namespace http_common {

using linukey::webserver::utils::SplitString;
using linukey::webserver::utils::Trim;

bool post_extract(const string& data,
                  unordered_map<string, string>& ret) {

    vector<string> lines = SplitString(data, "&");
    for (const auto& line : lines) {
        vector<string> kv = SplitString(line, "=");
        ret[kv[0]] = kv[1];
    }

    return true;
}

bool post_extract(const string& content_type,
                  const string& data,
                  unordered_map<string, string>& ret) {
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

}
}
}
