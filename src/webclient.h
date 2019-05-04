/*
    author: linukey
    time: 2019.04.04
*/

#ifndef __LINUKEY_WEBCLIENT_H__
#define __LINUKEY_WEBCLIENT_H__

#include <iostream>
#include <map>
#include "response.h"

using linukey::webserver::response::Response;

namespace linukey {
namespace webclient {

class WebClient {
public:
    /*
     * func : http客户端请求，支持https
     * 
     * url     : 请求url
     * method  : 请求方法
     * headers : 请求头
     * data    : 请求体
     *
     * Response : 响应报文信息
     *
     */
    static Response http_request(const std::string& url, const std::string& method, std::map<string, string>* headers, const std::string& data);

private:
    /*
     * func : 从url中提取host
     */
    static string extract_host(const std::string& url);
};

}
}

#endif
