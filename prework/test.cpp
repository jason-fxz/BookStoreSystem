#include <iostream>
#include <cstring>
#include "memoryriver.hpp"
// using namespace std;
namespace sjtu {

template <class T>
int Camp(const T &x, const T &y) {
    return (x == y) ? 0 : (x < y ? -1 : 1);
}

int Camp(const char *x, const char *y) {
    return strcmp(x, y);
}

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

    bool operator<(const String &rhs) const {
        return strcmp(data, rhs.data) < 0;
    }

    bool empty() const {
        return data[0] == '\0';
    }

    char &operator[](const int &k) {
        static_assert(0 <= k && k < len, "String [] out of bound");
        return data[k];
    }

    char operator[](const int &k) const {
        static_assert(0 <= k && k < len, "String [] out of bound");
        return data[k];
    }

    char *c_str() const {
        return data;
    }

    friend std::ostream &operator<<(std::ostream &os, const String &str) {
        os << str.data;
        return os;
    }

    friend std::istream &operator>>(std::istream &is, String &str) {
        is >> str.data;
        return is;
    }



};
typedef String<64> key_t;
typedef int value_t;
struct pair_t {
    key_t key;
    value_t value;

    pair_t() {}
    pair_t(const key_t &_key, const value_t &_value) : key(_key), value(_value) {}
    pair_t(const pair_t &rhs) : key(rhs.key), value(rhs.value) {}

    ~pair_t() = default;

    pair_t &operator=(const pair_t &rhs) {
        if (this == &rhs) return *this;
        key = rhs.key;
        value = rhs.value;
        return *this;
    }

    bool operator<(const pair_t &rhs) const {
        return Camp(key, rhs.key) == 0 ? value < rhs.value : Camp(key, rhs.key) < 0;
    }

    bool operator==(const pair_t &rhs) const {
        return Camp(key, rhs.key) == 0 && value == rhs.value;
    }

    friend bool operator<(const key_t &lhs, const pair_t &rhs) {
        return Camp(lhs, rhs.key) < 0;
    }

    friend std::ostream &operator<<(std::ostream &os, const pair_t &obj) {
        os << obj.key << " : " << obj.value;
        return os;
    }
};
struct Nodehead {
    int index; // index of Nodedata in the file
    int count; // count of pairs in this node
    pair_t max_pair; // max pair in this node
};
}

int main() {
    MemoryRiver<int> file("a.db");
    file.open();
    int tmp = 33;
    // file.write_info(4, 1);
    // file.write_info(10, 2);
    file.get_info(tmp, 1);
    std::cout << tmp << std::endl;
    file.get_info(tmp, 2);
    std::cout << tmp << std::endl;
    return 0;
}