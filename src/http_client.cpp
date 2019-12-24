#include "http_client.h"

#include <iostream>

#include <boost/array.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>

#include "log.h"
#include "utils.h"
#include "request.h"

using namespace std;
using boost::asio::ip::tcp;
using http::request::Request;

namespace http {
namespace httpclient {

bool HttpClient::extract_host_port(const string& url,
                                   string& protocol,
                                   string& host,
                                   string& port) {
    size_t pos = url.find("http://");
    int len = 7;

    if (pos == string::npos) {
        pos = url.find("https://");
        len = 8;
    }

    if (pos == string::npos) {
        return false;
    }

    protocol = url.substr(0, len-3);

    pos = url.find("/", len);
    if (pos == string::npos) {
        return false;
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
    } else {
        return false;
    }

    return true;
}

void HttpClient::build_request_message(const string& url,
                                       const string& method,
                                       const string& host,
                                       const string& data,
                                       map<string, string>* headers,
                                       Request& request) {
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
}

bool HttpClient::parse_response_line(const string& response_line,
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

    return response.Protocol().substr(0, 5) == "HTTP/";
}

template<class T>
void HttpClient::parse_response(T& socket,
                                Request& request,
                                Response& response) {
    try {
        // 发送请求
        socket.write_some(boost::asio::buffer(request.to_string()));

        boost::asio::streambuf response_streambuf;
        istream response_stream(&response_streambuf);

        // 解析状态行
        boost::asio::read_until(socket, response_streambuf, "\r\n");
        string response_line;
        getline(response_stream, response_line);

        boost::trim(response_line);
        parse_response_line(response_line, response);

        // 解析响应头
        boost::asio::read_until(socket, response_streambuf, "\r\n\r\n");
        string header_line;
        while (getline(response_stream, header_line) && header_line != "\r") {
            size_t pos = header_line.find(":");
            if (pos == string::npos) { continue; }
            string key = boost::trim_copy(header_line.substr(0, pos));
            string value = boost::trim_copy(header_line.substr(pos+1));
            response.setHeader(key, value);
        }

        // 接收包体
        string& response_body = response.setData();
        // chunked 方式
        if (response.Header("content-length").empty()) {
            if (response.Header("transfer-encoding") != "chunked") {
                LOGOUT(http::log::FATAL, "%", "no content-length and transfer-encoding");
                return;
            }

            int cur = 0, len = 0;
            size_t pos = string::npos;

            boost::asio::streambuf::const_buffers_type cbt = response_streambuf.data();
            string chunked_body(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
            response_streambuf.consume(chunked_body.size());

            while (true) {
                while ((pos = chunked_body.find("\r\n", cur)) == string::npos) {
                    boost::asio::read_until(socket, response_streambuf, "\r\n");
                    boost::asio::streambuf::const_buffers_type cbt = response_streambuf.data();
                    string read_str(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
                    chunked_body += read_str;
                    response_streambuf.consume(read_str.size());
                }
                stringstream ss;
                ss << hex << chunked_body.substr(cur, pos-cur);
                ss >> len;
                cur = cur + (pos-cur) + 2;
                while (chunked_body.size()-cur < len+2) {
                    int need_len = len + 2 - chunked_body.size() + cur;
                    boost::asio::read(socket, response_streambuf, boost::asio::transfer_at_least(need_len));
                    boost::asio::streambuf::const_buffers_type cbt = response_streambuf.data();
                    string read_str(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
                    chunked_body += read_str;
                    response_streambuf.consume(read_str.size());
                }
                response_body += chunked_body.substr(cur, len);
                if (len == 0) {
                    break;
                }
                cur = cur + 2 + len;
            }

        // content-length 方式
        } else {
            int content_length = stoi(response.Header("content-length"));
            boost::asio::streambuf::const_buffers_type cbt = response_streambuf.data();
            string first(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
            response_body += first;
            response_streambuf.consume(first.size());

            boost::asio::read(socket, response_streambuf, boost::asio::transfer_at_least(content_length - first.size()));

            cbt = response_streambuf.data();
            string rest(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
            response_body += rest;
            response_streambuf.consume(rest.size());
        }

        // 如果是gzip，解压
        if (boost::to_lower_copy(response.Header("Content-Encoding")) == "gzip") {
            string data = response.Data();
            string& decompress_data = response.setData();
            http::utils::gzip_decompress(data, decompress_data);
        }

        return;
    } catch (const exception& e) {
        LOGOUT(http::log::FATAL, "%", e.what());
        return;
    }
}

void HttpClient::connect_handler_http(tcp::socket& socket,
                                      Request& request,
                                      Response& response,
                                      const boost::system::error_code& error) {
    if (!error) {
        parse_response(socket, request, response);
    }
}

void HttpClient::connect_handler_https(boost::asio::ssl::stream<tcp::socket>& socket,
                                       Request& request,
                                       Response& response,
                                       const boost::system::error_code& error) {
    if (!error) {
        socket.handshake(boost::asio::ssl::stream_base::client);
        parse_response(socket, request, response);
    }
}

Response HttpClient::http_request(const string& url,
                                  const string& method,
                                  map<string, string>* headers,
                                  const string& data,
                                  int timeout) {
    Response response;
    string protocol, host, port;

    // 解析 协议头、host、port
    if (!extract_host_port(url, protocol, host, port)) {
        LOGOUT(http::log::FATAL, "% : % url=%", __func__, "not valid url!", url);
        return response;
    }

    // 支持 http、https 协议
    if (protocol != "http" && protocol != "https") {
        LOGOUT(http::log::FATAL, "protocol % error, only support http and https!", protocol);
        return response;
    }

    // 设置请求报文
    Request request;
    build_request_message(url, method, host, data, headers, request);

    try {
        boost::asio::io_context io_context;

        // 根据 hostname 查询 ip 地址
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

        // 如果没有查询到有效 ip 地址，直接写入请求的 ip 地址以及 port
        if (endpoint.address().to_string() == "0.0.0.0") {
            boost::asio::ip::address ad = boost::asio::ip::address::from_string(host);
            endpoint = boost::asio::ip::tcp::endpoint(ad, stoi(port));
        }


        // https
        if (protocol == "https") {
            boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
            ctx.set_verify_mode(boost::asio::ssl::verify_none);
            boost::asio::ssl::stream<tcp::socket> socket{io_context, ctx};

            // Set SNI Hostname (many hosts need this to handshake successfully)
            // https://github.com/boostorg/beast/blob/develop/example/http/client/sync-ssl/http_client_sync_ssl.cpp
            if(!SSL_set_tlsext_host_name(socket.native_handle(), host.c_str())) {
                boost::system::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
                    throw boost::system::system_error{ec};
            }

            socket.lowest_layer().async_connect(endpoint, boost::bind(&HttpClient::connect_handler_https,
                                                                      this,
                                                                      ref(socket),
                                                                      ref(request),
                                                                      ref(response),
                                                                      _1));
            // set timeout
            io_context.run_for(std::chrono::seconds(timeout));
            if (!io_context.stopped()) {
                LOGOUT(http::log::ERROR, "timeout url=%", url);
                socket.lowest_layer().close();
            }
        // http
        } else if (protocol == "http") {
            tcp::socket socket(io_context);
            socket.async_connect(endpoint, boost::bind(&HttpClient::connect_handler_http,
                                                       this,
                                                       ref(socket),
                                                       ref(request),
                                                       ref(response),
                                                       _1));
            // set timeout
            io_context.run_for(std::chrono::seconds(timeout));
            if (!io_context.stopped()) {
                LOGOUT(http::log::ERROR, "timeout url=%", url);
                socket.close();
            }
        }

        return response;
    } catch (const exception& e) {
        LOGOUT(http::log::FATAL, "%", e.what());
        return response;
    }
}

}}
