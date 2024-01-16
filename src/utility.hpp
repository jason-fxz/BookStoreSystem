// 常用函数，定义库

#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <regex>
#include <map>
#include <stdarg.h>
#include "string.hpp"
#include "exception.hpp"


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

std::string format(const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    return std::string(buffer);
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
    quit            = 0,  // quit
    exit            = 1,  // exit
    su              = 2,  // su [UserID] ([Password])?
    logout          = 3,  // logout
    regist          = 4,  // register [UserID] [Password] [Username]
    passwd          = 5,  // passwd [UserID] ([CurrentPassword])? [NewPassword]
    useradd         = 6,  // useradd [UserID] [Password] [Privilege] [Username]
    del             = 7,  // delete [UserID]
    show            = 8,  // show (-ISBN=[ISBN] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]")?
    buy             = 9,  // buy [ISBN] [Quantity]
    select          = 10, // select [ISBN]
    modify          = 11, // modify (-ISBN=[ISBN] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]" | -price=[Price])+
    import          = 12, // import [Quantity] [TotalCost]
    show_finance    = 13, // show finance ([Count])?
    log             = 14, // log
    report_finance  = 15, // report finance
    report_employee = 16, // report employee
    debug           = 100,// debug
    Undefined       = -1
};

std::map<std::string, Command_t> commandMap = {
    {"quit", Command_t::quit},
    {"exit", Command_t::exit},
    {"su", Command_t::su},
    {"logout", Command_t::logout},
    {"register", Command_t::regist},
    {"passwd", Command_t::passwd},
    {"useradd", Command_t::useradd},
    {"delete", Command_t::del},
    {"show", Command_t::show},
    {"buy", Command_t::buy},
    {"select", Command_t::select},
    {"modify", Command_t::modify},
    {"import", Command_t::import},
    {"show finance", Command_t::show_finance},
    {"log", Command_t::log},
    {"report finance", Command_t::report_finance},
    {"report employee", Command_t::report_employee},
    {"debug", Command_t::debug},
};

Command_t StringToCommand(const std::string &cmd) {
    auto it = commandMap.find(cmd);
    if (it != commandMap.end()) {
        return it->second;
    }
    return Command_t::Undefined;
}

// Split str by ch, store in res
template <size_t len>
void SplitString(const String<len> &str, std::vector<String<len>> &res,
                 const char &ch = ' ') {
    res.clear();
    std::string tmp;
    int size = strlen(str.c_str());
    bool flag_quote = 0; // when visit "
    for (int i = 0; i < size; ++i) {
        // if (str[i] == '\"') flag_quote ^= 1;
        if (str[i] == ch && flag_quote == 0) {
            if (!tmp.empty())  {
                res.push_back(String<len>(tmp));
                tmp.clear();
            }
        } else {
            tmp.push_back(str[i]);
        }
    }
    if (!tmp.empty()) res.push_back(String<len>(tmp));
}

// Split str by ch, store in res
void SplitString(const std::string &str, std::vector<std::string> &res,
                 const char &ch = ' ') {
    res.clear();
    std::string tmp;
    int size = strlen(str.c_str());
    bool flag_quote = 0; // when visit "
    for (int i = 0; i < size; ++i) {
        // if (str[i] == '\"') flag_quote ^= 1;
        if (str[i] == ch && flag_quote == 0) {
            if (!tmp.empty()) {
                res.push_back(tmp);
                tmp.clear();
            }
        } else {
            tmp.push_back(str[i]);
        }
    }
    if (!tmp.empty()) res.push_back(tmp);
}

bool isValidUserID(const std::string &str) {
    if (str.empty() || str.size() > 30) return false;
    for (int i = 0; i < str.size(); ++i) {
        if (!(str[i] >= '0' && str[i] <= '9' ||
              str[i] >= 'a' && str[i] <= 'z' ||
              str[i] >= 'A' && str[i] <= 'Z' ||
              str[i] == '_')) return false;
    }
    return true;
}

bool isValidPassword(const std::string &str) {
    if (str.empty() || str.size() > 30) return false;
    for (int i = 0; i < str.size(); ++i) {
        if (!(str[i] >= '0' && str[i] <= '9' ||
              str[i] >= 'a' && str[i] <= 'z' ||
              str[i] >= 'A' && str[i] <= 'Z' ||
              str[i] == '_')) return false;
    }
    return true;
}

bool isValidUsername(const std::string &str) {
    if (str.empty() || str.size() > 30) return false;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] < 32 || str[i] > 126) {
            return false;
        }
    }
    return true;
}

bool isValidPrivilege(const std::string &str) {
    if (str.size() != 1) return false;
    if (!(str[0] == '1' || str[0] == '3' || str[0] == '7')) return false;
    return true;
}

bool isValidISBN(const std::string &str) {
    if (str.empty() || str.size() > 20) return false;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] < 32 || str[i] > 126) {
            return false;
        }
    }
    return true;
}

bool isValidBookName(const std::string &str) {
    if (str.empty() || str.size() > 60) return false;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] < 32 || str[i] > 126 || str[i] == '\"') {
            return false;
        }
    }
    return true;
}

bool isValidAuthor(const std::string &str) {
    if (str.empty() || str.size() > 60) return false;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] < 32 || str[i] > 126 || str[i] == '\"') {
            return false;
        }
    }
    return true;
}

// flag = 1 表示不允许重复关键词， flag = 2 不允许多个关键词
bool isValidKeyword(const std::string &str, const int &flag = 1) {
    if (str.empty() || str.size() > 60) return false;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] < 32 || str[i] > 126 || str[i] == '\"') {
            return false;
        }
        if (i != 0 && str[i] == str[i - 1] && str[i] == '|') {
            return false;
        }
        if (flag == 2 && str[i] == '|') return false;
    }
    if (flag == 1) {
        std::vector<std::string> res;
        SplitString(str, res, '|');
        std::sort(res.begin(), res.end());
        for (int i = 1; i < res.size(); ++i) {
            
            if (res[i] == res[i - 1]) return false;
        }
    }
    return true;
}

bool isValidQuantity(const std::string &str) {
    if (str.empty() || str.size() > 10) return false;
    if (str[0] == '0') return false; // 前导零 / 数量为零
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] < '0' || str[i] > '9') {
            return false;
        }
    }
    return true;
}

bool isValidPrice(const std::string &str) {
    if (str.empty() || str.size() > 13) return false;
    if (str[0] == '.') return false;
    if (str.size() >= 2 && str[0] == '0' && str[1] != '.') return false;
    int countdot = 0;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] < '0' || str[i] > '9') {
            if (str[i] == '.') {
                if (++countdot > 1) return false;
            }
            else return false;
        }
    }
    if (countdot > 0 && str.back() == '.') return false;
    return true;
}

bool isValidCount(const std::string &str) {
    if (str.empty() || str.size() > 10) return false;
    if (str[0] == '0' && str.size() > 1) return false; // 前导零 
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] < '0' || str[i] > '9') {
            return false;
        }
    }
    return true;
}

bool getArgument(const std::string &str, const std::string &prefix, std::string &arg, const char& wrap = '\0') {
    if (str.size() < prefix.size()) return false;
    for (int i = 0; i < prefix.size(); ++i) {
        if (str[i] != prefix[i]) return false;
    }
    if (wrap == '\0') {
        arg = str.substr(prefix.size());
    } else {
        if (str.back() != wrap || str[prefix.size()] != wrap) return false;
        arg = str.substr(prefix.size() + 1, str.size() - prefix.size() - 2);
    }
    return true;
}
 





}

#endif // TOOLS_HPP