#include <iostream>
#include "../src/webclient.h"

using namespace std;
using namespace linukey::webclient;

int main () {
    const string url = "https://www.baidu.com";
    cout << WebClient::http_request(url, "GET", nullptr, "").to_string() << endl;
    

    return 0;
}
