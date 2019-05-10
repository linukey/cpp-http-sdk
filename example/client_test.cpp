#include <iostream>
#include "../src/webclient.h"

using namespace std;
using namespace linukey::webclient;

int main () {
    const string url = "http://106.12.110.100:8008/get_wechat_article?url=https://mp.weixin.qq.com/s/TLYkYOxRe3IjvmRYmYDEug&format=1";
    //const string url = "https://mp.weixin.qq.com:443/s/7Hw4af2Sk2aTR38StG69RQ";
    //const string url = "https://www.baidu.com/";
    WebClient webclient;
    cout << webclient.http_request(url, "GET", nullptr, "").to_string() << endl;
    

    return 0;
}
