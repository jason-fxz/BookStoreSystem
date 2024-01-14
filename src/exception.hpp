// my exception

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <string>

namespace acm {

class Exception {
protected:
    static std::string massage;
public:
    Exception() = default;
    Exception(const std::string &msg) {
        massage = msg;
    }
    ~Exception() = default;
    std::string &what() const {
        return massage;
    }
};


class PrivilegeError : public Exception {
public:
    PrivilegeError(const std::string &msg) : Exception("PrivilegeError: " + msg) {}
};

class UserError : public Exception {
public:
    UserError(const std::string &msg) : Exception("UserError: " + msg) {}
};

class CommandError : public Exception {
public:
    CommandError(const std::string &msg) : Exception("CommandError: " + msg) {}
};


}


#endif // EXCEPTION_HPP