#include "webclient.h"

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

string WebClient::extract_host(const string& url) {
    size_t pos = url.find("http://");
    int len = 7;

    if (pos == string::npos) {
        pos = url.find("https://");
        len = 8;
    }

    if (pos == string::npos) {
        return "";
    }

    pos = url.find("/", len);
    if (pos == string::npos) {
        return string();
    }

    return url.substr(len, pos-len);
}

Response WebClient::http_request(const string& url,
                                 const string& method,
                                 std::map<string, string>* headers,
                                 const string& data) {
    const string host = extract_host(url);
    if (host.empty()) {
        LOGOUT(webserver::log::FATAL, "%:%", __func__, "not valid url!");
        return Response();
    }

    try {
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(host, "https");

        // support https
        boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
        boost::asio::ssl::stream<tcp::socket> socket(io_context, ctx);
        socket.set_verify_mode(boost::asio::ssl::verify_none);
        boost::asio::connect(socket.lowest_layer(), endpoints);
        socket.handshake(boost::asio::ssl::stream_base::client);

        for (;;) {
            // 设置请求报文
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

            // 发送请求
            socket.write_some(boost::asio::buffer(request.to_string()));

            Response response;

            boost::asio::streambuf response_streambuf;
            boost::asio::read_until(socket, response_streambuf, "\r\n");

            // 解析状态行
            std::istream response_stream(&response_streambuf);
            string request_line;
            std::getline(response_stream, request_line);
            vector<string> ret;
            boost::split(ret, request_line, boost::is_any_of(" "));

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

            if (!response_stream || response.getProtocol().substr(0, 5) != "HTTP/") {
                LOGOUT(webserver::log::FATAL, "%", "error response protocol!");
                return Response();
            }

            boost::asio::read_until(socket, response_streambuf, "\r\n\r\n");

            // 解析响应头
            string header_line;
            while (std::getline(response_stream, header_line) && header_line != "\r") {
                size_t pos = header_line.find(":");
                if (pos == string::npos) {
                    LOGOUT(webserver::log::FATAL, "%", "error response header!");
                    return Response();
                }
                string key = boost::trim_copy(header_line.substr(0, pos));
                string value = boost::trim_copy(header_line.substr(pos+1));
                response.setHeader(key, value);
            }

            if (response.getStatusCode() != "200") {
                return response;
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
                        }
                    }
                }
                string response_body;

                int cur = 0;
                while (true) {
                    size_t pos = chunked_body.find("\r\n", cur);
                    if (pos == string::npos) {
                        break;
                    }
                    std::stringstream ss;
                    ss << std::hex << chunked_body.substr(cur, pos-cur);
                    int len;
                    ss >> len;
                    cur = cur + (pos-cur) + 2;
                    response_body += chunked_body.substr(cur, len);
                    cur = cur + 2 + len;
                }
                response.setData(response_body);

                //1\r\n a\r\n 2\r\n a b  \r \n 3  \r \n a  b  c  \r \n 0\r\n\r\n
                //0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20

            // content-length 方式
            } else {
                int body_len = 0;
                try {
                    body_len = stoi(response.getHeader("content-length"));
                } catch (const std::exception& e) {
                    LOGOUT(webserver::log::FATAL, "%", "error response content-length!");
                    return Response();
                }

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
        }

    } catch (std::exception& e) {
        LOGOUT(webserver::log::FATAL, "%", e.what());
    }

    return Response();
}

}}
