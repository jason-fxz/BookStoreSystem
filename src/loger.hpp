// 日志信息管理

#ifndef LOGER_HPP
#define LOGER_HPP

#include "fileio.hpp"
#include "utility.hpp"
#include <string>
#include <iostream>

namespace acm {

class Loger : public File {
    friend class LogSystem;
  private:
    static const int MAX_BUFFER_SIZE = 4096;
    std::string buffer;

  public:
    void flush() {
        file.clear();
        file.seekp(0, std::ios::end);
        file.write(buffer.c_str(), buffer.size());
        buffer.clear();
    }

    void write(const std::string &str) {
        buffer += str;
        if (buffer.size() > MAX_BUFFER_SIZE) flush();
    }

    void write(const char *str) {
        buffer += str;
        if (buffer.size() > MAX_BUFFER_SIZE) flush();
    }

    void write(const char &ch) {
        buffer += ch;
        if (buffer.size() > MAX_BUFFER_SIZE) flush();
    }

    void printAll() {
        flush();
        file.seekg(0, std::ios::beg);
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << "\n";
        }
        std::cout << std::endl;
        file.clear();
    }

    Loger(const char *file_name) : File(file_name) {
        if (!exist()) init();
        else {
            open();
            file.seekp(0, std::ios::end);
            file.seekg(0, std::ios::end);
            file.clear();
        }
    }
    ~Loger() {
        flush();
    }
};



}


#endif // LOGER_HPP