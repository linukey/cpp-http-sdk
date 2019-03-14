#include <iostream>
#include <unordered_map>
#include <fstream>
#include <vector>
#include "request.h"
#include "utils/file_utils.h"

using namespace linukey::webserver::utils;
using namespace linukey::webserver::request;
using namespace std;

int main() {
    string content_type = "content-type:multipart/form-data;boundary=----------------------------656139830131186291862329";
    unordered_map<string, string> m;

    string data = read_all("1");

    Request::post_extract(content_type, data, m);

    return 0;
}
