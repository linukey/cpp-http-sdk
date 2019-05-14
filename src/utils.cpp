/*
    author: linukey
    time: 2017.11.12
*/

#include "utils.h"

using namespace std;
using namespace boost::iostreams;

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

void gzip_compress(const string& text, string& out_text) {
    out_text.clear();
    filtering_ostream fos;
    fos.push(gzip_compressor(gzip_params(gzip::best_compression)));
    fos.push(boost::iostreams::back_inserter(out_text));
    fos << text;
    boost::iostreams::close(fos);
}

void gzip_decompress(const string& text, string& out_text) {
    out_text.clear();
    filtering_ostream fos;
    fos.push(gzip_decompressor());
    fos.push(boost::iostreams::back_inserter(out_text));
    fos << text;
    fos << std::flush;
}

void urldecode(const string& encd, string& decd) {
    int j;
    char p[2];
    unsigned int num;
    j=0;

    for(int i = 0; i < encd.size(); i++) {
        memset(p, '\0', 2);
        if(encd[i] != '%') {
            decd += encd[i];
            continue;
        }

        p[0] = encd[++i];
        p[1] = encd[++i];

        p[0] = p[0] - 48 - ((p[0] >= 'A') ? 7 : 0) - ((p[0] >= 'a') ? 32 : 0);
        p[1] = p[1] - 48 - ((p[1] >= 'A') ? 7 : 0) - ((p[1] >= 'a') ? 32 : 0);
        decd += (char)(p[0] * 16 + p[1]);
    }
}

std::string get_extension_from_url(const std::string& url) {
    for (int i = url.size()-1; i >=0; --i) {
        if (url[i] == '/') {
            return "";
        } else if (url[i] == '.') {
            return url.substr(i+1);
        }
    }

    return "";
};

}
}
}
