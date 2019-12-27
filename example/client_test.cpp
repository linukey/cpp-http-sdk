#include <iostream>
#include "../src/http_client.h"
#include "../src/utils.h"

using namespace std;
using namespace http::httpclient;
using namespace http::utils;

int main () {
    //const string url = "http://www.cneo.com.cn/"; //timeout
    //const string url = "http://www.gsei.com.cn/"; //timeout
    //const string url = "http://www.baidu.com/";
    //const string url = "http://www.huanqiu.com/"; // redirect
    //const string url = "https://www.huanqiu.com/";
    const string url = "http://www.zhongguojinrongtouziwang.com/";
    
    HttpClient http_client;

    map<string, string> headers;
    //headers["Accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9";
    headers["Accept-Encoding"] = "gzip";
    //headers["Accept-Language"] = "zh,zh-TW;q=0.9,zh-CN;q=0.8,en;q=0.7";
    //headers["User-Agent"] = "Mozilla/5.0 (Linux; Android 5.0; SM-G900P Build/LRX21T) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Mobile Safari/537.36";

    cout << http_client.http_request(url, "GET", &headers, "").to_string() << endl;

    return 0;
}
