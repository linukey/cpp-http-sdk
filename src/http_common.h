#ifndef __LINUKEY_HTTP_COMMON_H__
#define __LINUKEY_HTTP_COMMON_H__

#include <iostream>
#include <cstring>
#include <vector>

using std::string;
using std::vector;

namespace linukey {
namespace webserver {
namespace http_common {

static const string CRLF = "\r\n";
static const string SPACE = " ";

// 请求头
enum HEADERS{
    CONTENT_LENGTH = 0,
    CONTENT_TYPE,
    HOST,
    USER_AGENT,
    ACCEPT,
    ACCEPT_ENCODING,
    ACCEPT_LANGUAGE,
    CONNECTION,
    TRANSFER_ENCODING,
    HEADER_NUMS
};

static vector<string> HEADERS_STR = {
    "content-length",  
    "content-type",
    "host",
    "user-agent",
    "accept",
    "accept-encoding",
    "accept-language",
    "connection",
    "transfer-encoding"
};

static void urldecode(const string& encd, string& decd) {
    int j;
    char p[2];
    unsigned int num;
    j=0;

    for(int i = 0; i < encd.size(); i++) {
        memset(p, '\0', 2);
        if(encd[i] != '%') {
            decd += encd[i];
            continue;
        }

        p[0] = encd[++i];
        p[1] = encd[++i];

        p[0] = p[0] - 48 - ((p[0] >= 'A') ? 7 : 0) - ((p[0] >= 'a') ? 32 : 0);
        p[1] = p[1] - 48 - ((p[1] >= 'A') ? 7 : 0) - ((p[1] >= 'a') ? 32 : 0);
        decd += (char)(p[0] * 16 + p[1]);
    }
}

}
}
}

#endif
