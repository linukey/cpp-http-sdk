/*
    author: linukey
    time: 2017.11.12
    ps:
*/

#include "../include/webserver.h"
#include "../include/http.h"
#include "../include/log.h"
#include "../utils/string_utils.h"
#include "../utils/file_utils.h"
#include <fstream>
#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <cctype>
#include <vector>
#include <sstream>
#include <cstdio>

using namespace std;
using namespace boost::asio;
using namespace linukey::webserver;
using namespace linukey::webserver::http;
using namespace linukey::utils;
using namespace linukey::log;

WebServer::WebServer() : ACCEPTOR(SERVICE, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8001)) {
}

void WebServer::run(){    
    LOGOUT(INFO, "start server...");
    shared_socket sock(new ip::tcp::socket(SERVICE));
    ACCEPTOR.async_accept(*sock, boost::bind(&WebServer::accept_handle, this, sock, _1));
    SERVICE.run();
}

void WebServer::accept_handle(shared_socket sock, const e_code& err){
    if (err){
        LOGOUT(FATAL, "accept fatal");
        return;
    }

    char *buff = new char[buffer_size];       
    Request* req = new Request();

    ip::tcp::endpoint remote_ep = sock->remote_endpoint();
    ip::address remote_ad = remote_ep.address();
    req->host = remote_ad.to_string();

    async_read(*sock, 
               buffer(buff, buffer_size), 
               bind(&WebServer::read_complete, this, req, buff, _1, _2),
               bind(&WebServer::read_handle, this, req, sock, buff, _1, _2));

    run();
}

size_t WebServer::read_complete(Request* req, char *buff, const e_code& err, size_t size){
    if (err) {
        LOGOUT(FATAL, "receive request error");
        return 0;
    }

    string request(buff, size);
    if (req->method.empty()){
        if (request.find("\r\n\r\n") != string::npos){
            extract_request(request, req);
            if (to_lower(req->method) == "get") return false;
            else if (to_lower(req->method) == "post"){
                stringstream ss(req->headers[REQUEST_HEADERS_STR[CONTENT_LENGTH]]);
                int content_length;
                ss >> content_length;
                if (content_length == 0) return false;
                else return true;
            }
        } else return true;
    } else {
        string data = request.substr(request.find("\r\n\r\n") + 4);    
        stringstream ss(req->headers[REQUEST_HEADERS_STR[CONTENT_LENGTH]]);
        int content_length;
        ss >> content_length;
        if (data.size() == content_length) {
            extract_datas(data, req->datas);
            return false;
        } else return true;
    }
}

void WebServer::read_handle(Request* req, shared_socket sock, char *buff, const e_code& err, size_t size){
    if (err){
        LOGOUT(ERROR, "read handel error");
        return;
    }
    delete[] buff;

    LOGOUT(INFO, req->host + " request " + req->url + " ...");

    router(req, sock);
    sock->close();
}

void WebServer::write_handle(const e_code& err, size_t size){
    if (err){
        LOGOUT(ERROR, "write handel error");
        return;
    }
}

void WebServer::response(shared_socket sock, string message) {
    message = HEADER + message;
    sock->async_write_some(buffer(message), bind(&WebServer::write_handle, this, _1, _2));
}

void WebServer::router(Request* req, shared_socket sock){
    delete req;
}
