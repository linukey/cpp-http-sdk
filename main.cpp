/*
    author: linukey
    time: 2017.11.12
    ps:
*/

#include "include/webserver.h"
#include <iostream>

using namespace std;
using namespace linukey::webserver;
using namespace boost::asio;

class MyServer : public WebServer {
public:
    // 实现自己的路由
    void router(linukey::webserver::http::Request* req, shared_socket sock) override {
        if (req->url == "/get_proxy") {
            response(sock, "localhost");
        }
    }
};

int main(){
    MyServer server;
    server.run();
    
    return 0;
}
