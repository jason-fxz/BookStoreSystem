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

std::string acm::Exception::massage;

// // 用于
// class PrivilegeError : public Exception {
// public:
//     PrivilegeError(const std::string &msg) : Exception("PrivilegeError: " + msg) {}
// };

// 用于用户系统的错误
class UserSystemError : public Exception {
  public:
    UserSystemError(const std::string &msg) : Exception("UserSystemError: " +
                msg) {}
};

// 用于书籍系统的错误
class BookSystemError : public Exception {
  public:
    BookSystemError(const std::string &msg) : Exception("BookSystemError: " +
                msg) {}
};

// 用于命令格式输入错误
class CommandError : public Exception {
  public:
    CommandError(const std::string &msg) : Exception("CommandError: " + msg) {}
};

// 用于日志系统错误
class LogSystemError : public Exception {
  public:
    LogSystemError(const std::string &msg) : Exception("LogSystemError: " + msg) {}
};

// 用于服务器拒绝访问
class AuthorizationError : public Exception {
  public:
    AuthorizationError(const std::string &msg) : Exception("AuthorizationError: " + msg) {}
};

// 用于服务器 API 访问参数错误
class APIError : public Exception {
  public:
    APIError(const std::string &msg) : Exception("APIError: " + msg) {}
};

// Just for exit the program
class Exit : public Exception {
  public:
    Exit() = default;
};

}


#endif // EXCEPTION_HPP