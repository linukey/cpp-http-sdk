#include "http_client.h"

#include <iostream>

#include <boost/array.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>

#include "log.h"
#include "request.h"

using boost::asio::ip::tcp;
using linukey::webserver::request::Request;
using std::string;
using std::vector;

namespace linukey {
namespace webclient {

void WebClient::extract_host_port(const std::string& url,
                                  std::string& protocol,
                                  std::string& host,
                                  std::string& port) {
    size_t pos = url.find("http://");
    int len = 7;

    if (pos == string::npos) {
        pos = url.find("https://");
        len = 8;
    }

    if (pos == string::npos) {
        return;
    }

    protocol = url.substr(0, len-3);

    pos = url.find("/", len);
    if (pos == string::npos) {
        return;
    }

    size_t port_pos = url.find(":", len);

    if (port_pos != string::npos && port_pos < pos) {
        host = url.substr(len, port_pos-len);
        port = url.substr(port_pos+1, pos-port_pos-1);
    } else if (port_pos == string::npos || port_pos > pos) {
        host = url.substr(len, pos-len);
        if (protocol == "http") {
            port = "80";
        } else if (protocol == "https") {
            port = "443";
        }
    }
}

Request WebClient::build_request_message(const string& url,
                                         const string& method,
                                         const string& host,
                                         const string& data,
                                         std::map<string, string>* headers) {
    Request request;
    request.setMethod(method);
    request.setUrl(url);
    request.setHeader("Host", host);
    request.setProtocol("HTTP/1.1");
    request.setData(data);

    if (headers != nullptr) {
        for (auto it = headers->begin(); it != headers->end(); ++it) {
            request.setHeader(it->first, it->second);
        }
    }

    return request;
}

bool WebClient::parse_response_line(const string& response_line,
                                    Response& response) {
    vector<string> ret;
    boost::split(ret, response_line, boost::is_any_of(" "));

    response.setProtocol(ret[0]);
    response.setStatusCode(ret[1]);
    string status_des;
    for (int i = 2; i < ret.size(); ++i) {
        status_des += ret[i];
        if (i != ret.size()-1) {
            status_des += " ";
        }
    }
    response.setStatusDescribe(status_des);

    return response.getProtocol().substr(0, 5) == "HTTP/";
}

template <class T>
Response WebClient::parse_response_message(T& socket,
                                           const string& url,
                                           const string& method,
                                           const string& host,
                                           const string& data,
                                           std::map<string, string>* headers) {
    try {
        // 设置请求报文
        Request request = build_request_message(url, method, host, data, headers);

        // 发送请求
        socket.write_some(boost::asio::buffer(request.to_string()));

        Response response;
        boost::asio::streambuf response_streambuf;
        std::istream response_stream(&response_streambuf);

        // 解析状态行
        boost::asio::read_until(socket, response_streambuf, "\r\n");
        string response_line;
        std::getline(response_stream, response_line);
        parse_response_line(response_line, response);

        // 解析响应头
        boost::asio::read_until(socket, response_streambuf, "\r\n\r\n");
        string header_line;
        while (std::getline(response_stream, header_line) && header_line != "\r") {
            size_t pos = header_line.find(":");
            if (pos == string::npos) { continue; }
            string key = boost::trim_copy(header_line.substr(0, pos));
            string value = boost::trim_copy(header_line.substr(pos+1));
            response.setHeader(key, value);
        }

        // chunked 方式
        if (response.getHeader("content-length").empty()) {
            if (response.getHeader("transfer-encoding") != "chunked") {
                LOGOUT(webserver::log::FATAL, "%", "no content-length and transfer-encoding");
                return Response();
            }

            string chunked_body;
            boost::system::error_code ec;
            while (true) {
                boost::asio::read(socket, response_streambuf, boost::asio::transfer_at_least(1), ec);
                if (ec == boost::asio::error::eof) {
                    boost::asio::streambuf::const_buffers_type cbt = response_streambuf.data();
                    chunked_body = string(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
                    if (chunked_body.find("0\r\n\r\n") != string::npos) {
                        break;
                    } else {
                        LOGOUT(webserver::log::FATAL, "%:%:%", __FILE__, __LINE__, "error");
                        return Response();
                    }
                }
            }
            string response_body;

            int cur = 0;
            while (true) {
                size_t pos = chunked_body.find("\r\n", cur);
                if (pos == string::npos) { break; }
                std::stringstream ss;
                ss << std::hex << chunked_body.substr(cur, pos-cur);
                int len;
                ss >> len;
                cur = cur + (pos-cur) + 2;
                response_body += chunked_body.substr(cur, len);
                cur = cur + 2 + len;
            }
            response.setData(response_body);
        // content-length 方式
        } else {
            int body_len = stoi(response.getHeader("content-length"));
            while (true) {
                boost::asio::read(socket, response_streambuf, boost::asio::transfer_at_least(1));
                if (response_streambuf.size() == body_len) {
                    boost::asio::streambuf::const_buffers_type cbt = response_streambuf.data();
                    string data(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
                    response.setData(data);
                    break;
                }
            }
        }

        return response;
    } catch (std::exception& e) {
        LOGOUT(webserver::log::FATAL, "%", e.what());
        return Response();
    }

    return Response();
}

Response WebClient::http_request(const string& url,
                                 const string& method,
                                 std::map<string, string>* headers,
                                 const string& data) {
    string protocol, host, port;
    extract_host_port(url, protocol, host, port);

    if (host.empty() || protocol.empty()) {
        LOGOUT(webserver::log::FATAL, "%:%", __func__, "not valid url!");
        return Response();
    }

    if (protocol != "http" && protocol != "https") {
        LOGOUT(webserver::log::FATAL, "protocol % error, only support http and https!", protocol);
        return Response();
    }

    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(host, protocol);
        boost::asio::ip::tcp::endpoint endpoint;
        for (auto it = endpoints.begin(); it != endpoints.end(); ++it) {
            boost::asio::ip::tcp::endpoint ep = *it;
            if (ep.port() == stoi(port)) {
                endpoint = ep;
                break;
            }
        }

        if (endpoint.address().to_string() == "0.0.0.0") {
            boost::asio::ip::address ad = boost::asio::ip::address::from_string(host);
            endpoint = boost::asio::ip::tcp::endpoint(ad, stoi(port));
        }

        if (protocol == "https") {
            boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
            boost::asio::ssl::stream<tcp::socket> socket(io_context, ctx);
            socket.set_verify_mode(boost::asio::ssl::verify_none);
            socket.lowest_layer().connect(endpoint);
            socket.handshake(boost::asio::ssl::stream_base::client);
            return parse_response_message(socket, url, method, host, data, headers);

        } else if (protocol == "http") {
            tcp::socket socket(io_context);
            socket.connect(endpoint);
            return parse_response_message(socket, url, method, host, data, headers);
        }

        return Response();
    } catch (std::exception& e) {
        LOGOUT(webserver::log::FATAL, "%", e.what());
        return Response();
    }
}

}}
