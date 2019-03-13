#include <iostream>
#include <unordered_map>
#include <fstream>
#include <vector>
#include "http_common.h"
#include "utils/file_utils.h"

using linukey::webserver::http_common::HttpCommon;
using namespace linukey::webserver::utils;
using namespace std;

int main() {
    string content_type = "content-type:multipart/form-data;boundary=----------------------------656139830131186291862329";
    unordered_map<string, string> m;

    string data = read_all("1");

    HttpCommon::post_extract(content_type, data, m);

    return 0;
}
