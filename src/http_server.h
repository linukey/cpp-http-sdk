/*
    author: linukey
    time: 2017.11.12
*/

#ifndef __LINUKEY_WEBSERVER_H__
#define __LINUKEY_WEBSERVER_H__

#include "request.h"
#include "http_common.h"

#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <cctype>
#include <vector>
#include <cstdio>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using std::shared_ptr;
using std::string;
using std::map;
using linukey::webserver::request::Request;

using namespace linukey::webserver::http_common;

namespace linukey{  
namespace webserver{

typedef shared_ptr<boost::asio::ip::tcp::socket> shared_socket;
typedef boost::system::error_code e_code;

class WebServer{    
public:
    WebServer(int buffer_size, int port);
    boost::asio::io_service SERVICE;
    boost::asio::ip::tcp::acceptor ACCEPTOR;

    // 响应 (默认)
    void response(shared_ptr<Request> req, shared_socket sock, const string& message);

    // 响应 (自定义header)
    void response(shared_ptr<Request> req, shared_socket sock, const string& header, const string& message);

    // 响应 (chunked)
    void response_chunked(shared_ptr<Request> req, shared_socket sock, const string& message);

    // 启动
    void run();

    // 加载配置文件
    static bool read_conf(const string& file_path, map<string, string>& g_conf);

private:
    void accept();

    const int buffer_size;

    void accept_handle(shared_socket sock, const e_code& err);

    void write_handle(const e_code& err);

    void read_handle(shared_ptr<Request> req,
                     shared_socket sock,
                     const e_code& err);

    size_t read_complete(shared_ptr<Request> req,
                         shared_ptr<char> buff,
                         const e_code& err,
                         size_t size);

// 业务端实现
protected:
    // 路由
    virtual void router(shared_ptr<Request> req, shared_socket sock) = 0;
};

} // namespace webserver
} // namespace linukey

#endif
