// 一个简单固定长度的封装 char[]

#ifndef STRING_HPP
#define STRING_HPP

#include <fstream>
#include <iomanip>

namespace acm {

template <size_t len>
class String {
  private:
    char data[len];
  public:

    String() { memset(data, 0, sizeof(data)); }
    String(const char *str) {
        strcpy(data, str);
    }
    String(const String &rhs) {
        memcpy(data, rhs.data, sizeof(data));
    }
    String(const std::string &rhs) {
        memcpy(data, rhs.c_str(), sizeof(data));
    }
    ~String() = default;

    String &operator=(const String &rhs) {
        if (this == &rhs) return *this;
        memcpy(data, rhs.data, sizeof(data));
        return *this;
    }

    String &operator=(const char *str) {
        memset(data, 0, sizeof(data));
        strcpy(data, str);
        return *this;
    }

    bool operator==(const String &rhs) const {
        return strcmp(data, rhs.data) == 0;
    }

    bool operator!=(const String &rhs) const {
        return strcmp(data, rhs.data) != 0;
    }

    bool operator<(const String &rhs) const {
        return strcmp(data, rhs.data) < 0;
    }

    bool empty() const {
        return data[0] == '\0';
    }

    void clear() {
        memset(data, 0, sizeof(data));
    }

    char &operator[](const int &k) {
        // static_assert(0 <= k && k < len, "String [] out of bound");
        return data[k];
    }

    char operator[](const int &k) const {
        // static_assert(0 <= k && k < len, "String [] out of bound");
        return data[k];
    }

    const char *c_str() const {
        return data;
    }

    friend std::ostream &operator<<(std::ostream &os, const String &str) {
        os << std::setw(len) << str.data;
        return os;
    }

    friend std::istream &operator>>(std::istream &is, String &str) {
        is >> str.data;
        return is;
    }
};


}



#endif // STRING_HPP