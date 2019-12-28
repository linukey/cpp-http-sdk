/*
    author: linukey
    time: 2019.04.04
*/

#ifndef __HTTP_HTTPCLIENT_H__
#define __HTTP_HTTPCLIENT_H__

#include <iostream>
#include <map>
#include "response.h"
#include "request.h"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

using http::response::Response;
using http::request::Request;

namespace http {
namespace httpclient {

struct Socket {
    boost::asio::ip::tcp::socket socket;
};

class HttpClient {
public:
    /*
     * func : http客户端请求，支持https
     * 
     * url     : 请求url
     * method  : 请求方法
     * headers : 请求头
     * data    : 请求体
     * timeout : 超时
     *
     * Response : 响应报文信息
     *
     */
    Response http_request(const std::string& url,
                          const std::string& method,
                          std::map<string, string>* headers,
                          const std::string& data,
                          int timeout = 2,
                          int redirect_count = 0);
private:
    /*
     * func : 从url中提取host
     */
    bool extract_host_port(std::string url,
                           std::string& protocol,
                           std::string& host,
                           std::string& port);
    /*
     * 构建请求报文
     */
    void build_request_message(const std::string& url,
                               const std::string& method,
                               const std::string& host,
                               const std::string& data,
                               std::map<std::string,
                               std::string>* headers,
                               Request& request);
    /*
     * 处理响应报文
     */
    template<class T>
    void parse_response(boost::asio::io_context& io_context,
                        T& socket,
                        Request& request,
                        Response& response,
                        int timeout);
    /*
     * 解析响应报文状态行
     */
    bool parse_response_line(const string& response_line, Response& response);

    /*
     * 处理http_code 3xx
     */
    string build_redirection_url(const string& protocol,
                                 const string& host,
                                 Response& response);
    /*
     * 异步操作超时限制
     */
    bool timelimit(boost::asio::io_context& io_context, int timeout);
};

}}

#endif
