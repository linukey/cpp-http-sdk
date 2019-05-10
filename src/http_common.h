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

static vector<string> HEADERS = {
    "content-length",  
    "content-type",
    "host",
    "user-agent",
    "accept",
    "accept-encoding",
    "accept-language",
    "connection",
    "transfer-encoding",
    "content-encoding"
};

static const string RESPONSE_SUCCESS_STATUS_LINE = "HTTP/1.1 200 OK\r\n";

}
}
}

#endif
