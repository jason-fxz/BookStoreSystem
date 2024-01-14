// 命令行管理

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <regex>
#include "utility.hpp"
#include "system.hpp"

namespace acm {
class CommandManager {
  private:
    std::string input;

    BookSystem Books;
    UserSystem Users;
    LogSystem  Logs;


  public:
    CommandManager() = default;
    ~CommandManager() = default;

    void NextCommand() {
        
    }
};

}


#endif // COMMAND_HPP