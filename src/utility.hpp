// 常用函数，定义库

#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <cstring>
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



}

#endif // TOOLS_HPP