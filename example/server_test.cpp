/*
    author: linukey
    time: 2017.11.12
*/

#include "../src/http_server.h"
#include <iostream>
#include <map>
#include <fstream>
#include <memory>

using namespace std;
using namespace linukey::webserver;

map<string, string> g_conf;

string read_file(fstream& fin) {
    string first_line;
    getline(fin, first_line);
    if (first_line.empty()) {
        return "";
    }
    fin.seekg(0, ios::end);
    int length = int(fin.tellg()) - first_line.length();
    fin.seekg(first_line.length(), ios::beg);
    shared_ptr<char> buf(new char[length]);
    fin.read(buf.get(), length);
    fin.close();
    return string(buf.get(), length);
}

/*
 * 通过继承WebServer类，重写router方法，实现自己的业务端路由
 * 可以快速搭建一个本地http-server，而不必去关心http-server的底层实现
 */
class MyServer : public WebServer {
public:
    MyServer(int buffer_size, int port) : 
        WebServer(buffer_size, port) {}

    // 实现自己的业务路由
    void router(shared_ptr<Connection> conn) override {
        if (conn->request->getUrl() == "/") {
            fstream fout("html/index.html");
            if (fout.is_open()) {
                string html = read_file(fout);
                response(conn, html);
                fout.close();
            }
            return;
        }

        response(conn, "404");
    }
};

/*
 * 这个demo用于示范怎样通过继承WebServer类，实现自己的业务路由,
 * 来快速搭建一个本地可用的http-server
 * 可以修改这个文件，来实现自己的业务路由
 */
int main(){
    if (!MyServer::read_conf("webserver.conf", g_conf)) {
        cerr << "read conf fail!" << endl;
        return 0;
    }

    int buffer_size;
    int port;
    try {
        if (g_conf.count("buffer_size") == 0) {
            cerr << "conf param buffer_size not find!" << endl;
            return 0;
        }
        buffer_size = stoi(g_conf["buffer_size"]);
        if (g_conf.count("port") == 0) {
            cerr << "conf param port not find!" << endl;
            return 0;
        }
        port = stoi(g_conf["port"]);
    } catch (exception e) {
        cerr << "stoi fail" << endl;
        return 0;
    }

    MyServer server(buffer_size, port);
    server.run();
    
    return 0;
}
