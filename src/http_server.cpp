/*
    author: linukey
    time: 2017.11.12
*/

#include <boost/algorithm/string.hpp>
#include "http_server.h"
#include "request.h"
#include "log.h"
#include "http_utils.h"
#include "http_common.h"

using namespace boost::asio;
using namespace linukey::webserver;
using namespace linukey::webserver::request;
using namespace linukey::webserver::log;
using namespace linukey::webserver::utils;
using namespace linukey::webserver::http_common;

WebServer::WebServer(int buffer_size, int port) : 
    ACCEPTOR(SERVICE, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    buffer_size(buffer_size) {
}

void WebServer::run(){    
    LOGOUT(INFO, "%", "start server...");
    accept();
    SERVICE.run();
}

void WebServer::accept() {
    shared_socket sock(new ip::tcp::socket(SERVICE));
    ACCEPTOR.async_accept(*sock, boost::bind(&WebServer::accept_handle, this, sock, _1));
}

void WebServer::accept_handle(shared_socket sock, const e_code& err){
    if (err){
        LOGOUT(FATAL, "%", "accept fatal");
        return;
    }

    shared_ptr<char> buff(new char[buffer_size]);
    shared_ptr<Request> req(new Request());

    async_read(*sock, 
               buffer(buff.get(), buffer_size), 
               bind(&WebServer::read_complete, this, req, buff, _1, _2),
               bind(&WebServer::read_handle, this, req, sock, _1));

    accept();
}

// 逐个字节的读
size_t WebServer::read_complete(shared_ptr<Request> req, shared_ptr<char> buff, const e_code& err, size_t size){
    if (err) {
        LOGOUT(FATAL, "%", "receive request error");
        return 0;
    }

    string request(buff.get(), size);
    size_t pos = request.find(CRLF + CRLF);
    if (pos == string::npos) {
        return true;
    }

    if (req->getMethod().empty()){
        req->extract_request(request);
        if (boost::algorithm::to_lower_copy(req->getMethod()) == "get") {
            return false;
        } else if (boost::algorithm::to_lower_copy(req->getMethod()) == "post") {
            return stoi(req->getHeader(HEADERS_STR[CONTENT_LENGTH])) != 0;
        }
    } else {
        string data = request.substr(pos + 4);    
        int content_length = stoi(req->getHeader(HEADERS_STR[CONTENT_LENGTH]));
        if (int(data.size()) == content_length) {
            req->setData(data);
            return false;
        }
    }

    return true;
}

void WebServer::read_handle(shared_ptr<Request> req, shared_socket sock, const e_code& err){
    if (err){
        LOGOUT(ERROR, "%", "read handel error");
        return;
    }

    LOGOUT(INFO, "% request % ...", req->getHeader(HEADERS_STR[HOST]), req->getUrl());

    router(req, sock);
    sock->close();
}

void WebServer::write_handle(const e_code& err){
    if (err){
        LOGOUT(ERROR, "%", "write handel error");
        return;
    }
}

void WebServer::response_chunked(shared_socket sock, const string& message) {
    string message_ = message;
    std::string response_str = HEADER + "Transfer-Encoding: chunked" + "\r\n\r\n";
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
    sock->async_write_some(buffer(response_str), bind(&WebServer::write_handle, this, _1));
}

void WebServer::response(shared_socket sock, const string& message) {
    std::string header = HEADER + "Content-Length:" + std::to_string(message.size()) + "\r\n\r\n";
    sock->async_write_some(buffer(header + message), bind(&WebServer::write_handle, this, _1));
}

void WebServer::response(shared_socket sock, const string& header, const string& message) {
    sock->async_write_some(buffer(header + message), bind(&WebServer::write_handle, this, _1));
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
        string key = line.substr(0, split_pos);
        string value = line.substr(split_pos+1);
        g_conf[key] = value;
    }
    return true;
}
