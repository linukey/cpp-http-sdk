/*
    author: linukey
    time: 2017.11.12
*/

#include "../include/webserver.h"
#include "../include/http.h"
#include "../include/log.h"
#include "../include/utils/string_utils.h"
#include "../include/utils/file_utils.h"

using namespace boost::asio;
using namespace linukey::webserver;
using namespace linukey::webserver::http;
using namespace linukey::webserver::log;
using namespace linukey::webserver::utils;

WebServer::WebServer(int buffer_size, int port) : 
    ACCEPTOR(SERVICE, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    buffer_size(buffer_size)
{}

void WebServer::run(){    
    LOGOUT(INFO, "start server...");
    accept();
    SERVICE.run();
}

void WebServer::accept() {
    shared_socket sock(new ip::tcp::socket(SERVICE));
    ACCEPTOR.async_accept(*sock, boost::bind(&WebServer::accept_handle, this, sock, _1));
}

void WebServer::accept_handle(shared_socket sock, const e_code& err){
    if (err){
        LOGOUT(FATAL, "accept fatal");
        return;
    }

    shared_ptr<char> buff(new char[buffer_size]);
    shared_ptr<Request> req(new Request());

    ip::tcp::endpoint remote_ep = sock->remote_endpoint();
    ip::address remote_ad = remote_ep.address();
    req->host = remote_ad.to_string();

    async_read(*sock, 
               buffer(buff.get(), buffer_size), 
               bind(&WebServer::read_complete, this, req, buff, _1, _2),
               bind(&WebServer::read_handle, this, req, sock, _1));

    run();
}

// 逐个字节的读
size_t WebServer::read_complete(shared_ptr<Request> req, shared_ptr<char> buff, const e_code& err, size_t size){
    if (err) {
        LOGOUT(FATAL, "receive request error");
        return 0;
    }

    string request(buff.get(), size);
    size_t pos = request.find("\r\n\r\n");
    if (pos == string::npos) {
        return true;
    }

    if (req->method.empty()){
        extract_request(request, req);
        if (to_lower(req->method) == "get") {
            return false;
        } else if (to_lower(req->method) == "post") {
            return stoi(req->headers[REQUEST_HEADERS_STR[CONTENT_LENGTH]]) != 0;
        }
    } else {
        string data = request.substr(pos + 4);    
        int content_length = stoi(req->headers[REQUEST_HEADERS_STR[CONTENT_LENGTH]]);
        if (int(data.size()) == content_length) {
            extract_datas(data, req->datas);
            return false;
        }
    }

    return true;
}

void WebServer::read_handle(shared_ptr<Request> req, shared_socket sock, const e_code& err){
    if (err){
        LOGOUT(ERROR, "read handel error");
        return;
    }

    LOGOUT(INFO, req->host + " request " + req->url + " ...");

    router(req, sock);
    sock->close();
}

void WebServer::write_handle(const e_code& err){
    if (err){
        LOGOUT(ERROR, "write handel error");
        return;
    }
}

void WebServer::response(shared_socket sock, string message) {
    message = HEADER + message;
    sock->async_write_some(buffer(message), bind(&WebServer::write_handle, this, _1));
}
