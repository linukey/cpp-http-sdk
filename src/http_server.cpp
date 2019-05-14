/*
    author: linukey
    time: 2017.11.12
*/

#include <boost/algorithm/string.hpp>
#include "http_server.h"
#include "request.h"
#include "log.h"
#include "utils.h"
#include "http_common.h"

using namespace boost::asio;
using namespace linukey::webserver;
using namespace linukey::webserver::request;
using namespace linukey::webserver::log;
using namespace linukey::webserver::utils;
using namespace linukey::webserver::http_common;
using boost::asio::ip::tcp;

WebServer::WebServer(int buffer_size, int port) : 
                                                ACCEPTOR(SERVICE),
                                                buffer_size(buffer_size) {
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), port);
    ACCEPTOR.open(ep.protocol());
    ACCEPTOR.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    ACCEPTOR.bind(ep);
    ACCEPTOR.listen();
}

void WebServer::run(){    
    LOGOUT(INFO, "%", "start server...");
    accept();
    SERVICE.run();
}

void WebServer::accept() {
    shared_ptr<Connection> conn(new Connection(SERVICE, buffer_size));
    ACCEPTOR.async_accept(*conn->sock, boost::bind(&WebServer::accept_handle, this, conn, _1));
}

void WebServer::accept_handle(shared_ptr<Connection> conn, const e_code& err){
    if (err){
        LOGOUT(FATAL, "%", "accept fatal");
        return;
    }

    async_read(*conn->sock, 
               buffer(conn->request_buffer, buffer_size), 
               bind(&WebServer::read_complete, this, conn, _1, _2),
               bind(&WebServer::read_handle, this, conn, _1, _2));

    accept();
}

// 逐个字节的读
size_t WebServer::read_complete(shared_ptr<Connection> conn, const e_code& err, size_t size){
    if (err) {
        LOGOUT(FATAL, "%", "receive request error");
        return 0;
    }

    // 读取完整的 请求行 + 请求头
    string request(conn->request_buffer, size);
    size_t pos = request.find(CRLF + CRLF);
    if (pos == string::npos) {
        return true;
    }

    // 根据 method 类型决定是否继续读取 请求体
    if (conn->request->getMethod().empty()){
        conn->request->extract_request(request);
        if (boost::algorithm::to_lower_copy(conn->request->getMethod()) == "get") {
            return false;
        } else if (boost::algorithm::to_lower_copy(conn->request->getMethod()) == "post") {
            return stoi(conn->request->getHeader("content-length")) != 0;
        }
    } else {
        string body = request.substr(pos + 4);    
        int content_length = stoi(conn->request->getHeader("content-length"));
        if (int(body.size()) == content_length) {
            conn->request->setData(body);
            return false;
        }
    }

    return true;
}

void WebServer::read_handle(shared_ptr<Connection> conn,
                            const e_code& err,
                            std::size_t bytes_transferred){
    boost::asio::ip::tcp::endpoint endpoint = conn->sock->remote_endpoint();
    if (err){
        LOGOUT(ERROR, "%", "read handel error");
        return;
    }

    LOGOUT(INFO, "% request % ...", conn->request->getHeader("host"), conn->request->getUrl());

    router(conn);
}

void WebServer::write_handle(shared_ptr<Connection> conn,
                             const e_code& err,
                             std::size_t bytes_transferred){
    if (err){
        LOGOUT(ERROR, "%", "write handel error");
    }
    conn->sock->close();
}

/*
void WebServer::response_chunked(shared_ptr<Connection> conn, const string& message) {
    string message_ = message;
    std::string response_str = RESPONSE_SUCCESS_STATUS_LINE + "Transfer-Encoding: chunked" + "\r\n\r\n";
    while (message_.size() > 0) {
        if (message_.size() > 10000) {
            string block = message_.substr(0, 10000);
            message_ = message_.substr(10000);
            response_str += "2710\r\n";
            response_str += block + "\r\n";
        } else {
            char buffer[256];
            sprintf(buffer, "%x", message_.size());
            response_str += string(buffer) + "\r\n";
            response_str += message_ + "\r\n";
            message_.clear();
        }
    }
    response_str += "0\r\n\r\n";
    conn->response_buffer = response_str;
    async_write(*conn->sock, buffer(response_str), bind(&WebServer::write_handle, this, conn, _1, _2));
}
*/

void WebServer::response(shared_ptr<Connection> conn, const string& message) {
    string url = conn->request->getUrl();
    string& ret = conn->response_buffer;
    ret += RESPONSE_SUCCESS_STATUS_LINE; 

    string* body = new string;

    if (boost::to_lower_copy(conn->request->getHeader("Accept-Encoding")).find("gzip") != string::npos) {
        gzip_compress(message, *body);
        ret += "Content-Encoding:gzip\r\n"; 
    } else {
        body = (string*)&message;
    }

    std::string extension = get_extension_from_url(conn->request->getUrl());
    std::string type = extension_to_type(extension);

    if (!type.empty()) {
        ret += "Content-type:" + type + "\r\n";
    }

    ret += "Content-Length:" + std::to_string(body->size()) + "\r\n";
    ret += "\r\n";
    ret += *body;

    async_write(*conn->sock, buffer(conn->response_buffer), bind(&WebServer::write_handle, this, conn, _1, _2));
}

bool WebServer::read_conf(const string& file_path, std::map<string, string>& g_conf) {
    std::fstream fout(file_path);
    if (!fout.is_open()) {
        return false;
    }
    string line;
    while (std::getline(fout, line)) {
        if (line.empty() || line.find("#") == 0) {
            continue;
        }
        size_t split_pos = line.find(":");
        if (split_pos == string::npos) {
            return false;
        }
        string key = boost::trim_copy(line.substr(0, split_pos));
        string value = boost::trim_copy(line.substr(split_pos+1));
        g_conf[key] = value;
    }
    return true;
}
