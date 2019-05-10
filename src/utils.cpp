/*
    author: linukey
    time: 2017.11.12
*/

#include "utils.h"

using namespace std;

namespace linukey {
namespace webserver {
namespace utils {

void file_read_all(const string& filename, string& buffer){
    ifstream fin(filename);
    if (fin.is_open()){
        fin.seekg(0, ios::end);
        int length = fin.tellg();
        fin.seekg(0, ios::beg);
        char* buff = new char[length];
        fin.read(buff, length);
        fin.close();
        buffer = string(buff, length);
    }
}

}
}
}
