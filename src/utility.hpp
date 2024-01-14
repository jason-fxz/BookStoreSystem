// 常用函数，定义库

#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "string.hpp"

namespace acm {

using std::ostream;
using std::istream;
using std::fstream;

// Camp(x, y) 统一比较函数
template <class T>
int Camp(const T &x, const T &y) {
    return (x == y) ? 0 : (x < y ? -1 : 1);
}

int Camp(const char *x, const char *y) {
    return strcmp(x, y);
}

template <class T>
T Abs(const T &x) {
    return (x > 0) ? x : -x;
}

// Books
typedef String<22> ISBN_t;
typedef String<62> BookName_t;
typedef String<62> Author_t;
typedef String<62> Keyword_t;

// Users
typedef String<32> UserID_t;
typedef String<32> UserName_t;
typedef String<32> Password_t;
enum Privilege_t {
    eGuest    = 0,
    eCustomer = 1,
    eAdmin    = 3,
    eRoot     = 7
};

enum Search_t {
    sAll     = 0,
    sISBN    = 1,
    sName    = 2,
    sAuthor  = 3,
    sKeyword = 4,
    sPrice   = 5,
};


enum class Command_t {
    quit            = 0,
    exit            = 1,
    su              = 2,
    logout          = 3,
    regist          = 4,
    passwd          = 5,
    useradd         = 6,
    del             = 7,
    show            = 8,
    buy             = 9,
    select          = 10,
    modify          = 11,
    import          = 12,
    show_finance    = 13,
    log             = 14,
    report_finance  = 15,
    report_employee = 16
};

Command_t Convert(std::string cmd) {
    

}

// Split str by ch, store in res
template <size_t len>
void SplitString(const String<len> &str, std::vector<String<len>> &res,
                 const char &ch) {
    res.clear();
    std::string tmp;
    int size = strlen(str.c_str());
    for (int i = 0; i < size; ++i) {
        if (str[i] == ch) {
            res.push_back(String<len>(tmp));
            tmp.clear();
        } else {
            tmp.push_back(str[i]);
        }
    }
    if (!tmp.empty()) res.push_back(tmp);
}



}

#endif // TOOLS_HPP