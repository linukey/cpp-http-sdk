#ifndef __LINUKEY_LOG_H__
#define __LINUKEY_LOG_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <sstream>
#include <string>

using std::string;

namespace http {
namespace log {

enum LOG_LEVEL
{
    TRACE = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

const std::vector<string> LOG_LEVEL_STR
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

const string positive_log_file="webserver.log";
const string negative_log_file="webserver.log.wf";

static void msnprintf(string& buffer, string pattern){}
template <class T, class ...Args>
static void msnprintf(string& buffer, string pattern, T head, Args... rest) 
{
    size_t pos = pattern.find("%");
    while (pos != string::npos && pos-1 >= 0 && pattern[pos-1] == '\\') {
        pattern = pattern.substr(0, pos-1) + pattern.substr(pos);
        pos = pattern.find("%", pos);
    }

    if (pos == string::npos) { throw; }

    std::stringstream istr;
    istr << head;
    buffer += pattern.substr(0, pos) + istr.str();

    string rest_pattern = pattern.substr(pos+1);

    if (sizeof...(rest) == 0) {
        pos = rest_pattern.find("%");
        if (pos != string::npos && pos-1 >= 0 && rest_pattern[pos-1] != '\\') { 
            throw; 
        } else if (pos != string::npos) {
            rest_pattern = rest_pattern.substr(0, pos-1) + rest_pattern.substr(pos);
        }
        buffer += rest_pattern;
    }

    msnprintf(buffer, rest_pattern, rest...);
}

template <class T, class ...Args>
static void LOGOUT(LOG_LEVEL level, T head, Args... rest)
{
    string log_file;
    switch (level){
        case TRACE:
        case DEBUG:
        case INFO:
            log_file = positive_log_file;
        break;
        case WARN:
        case ERROR:
        case FATAL:
            log_file = negative_log_file;
        break;
        default:
            log_file = negative_log_file;
    };

    std::ofstream fout(log_file, std::ios::app);
    if (!fout.is_open()) {
        std::cerr << "open log file fatal!" << std::endl;
    }

    time_t rawtime;
    time(&rawtime);
    string time_str(ctime(&rawtime));
    time_str = time_str.substr(0, time_str.size()-1);

    string buffer;
    msnprintf(buffer, head, rest...);

    fout << LOG_LEVEL_STR[level] << ":" << time_str << " " << buffer << "\n";
    fout.close();
}

}}

#endif
