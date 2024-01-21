// 命令行管理

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "exception.hpp"
#include "user.hpp"
#include "utility.hpp"
#include "system.hpp"
#include <functional>
#include <stdexcept>
#include <string>


namespace acm {

std::map<std::string, const char *> reg = {
    {"UserID", "[\\dA-Za-z_]{1,30}"},
    {"Password", "[\\dA-Za-z_]{1,30}"},
    {"Username", "[\\x21-\\x7E]{1,30}"},
    {"Privilege", "[137]"},
    {"ISBN", "[\\x21-\\x7E]{1,20}"},
    {"BookName", "[\\x21-\\x21\\x23-\\x7E]{1,60}"},
    {"Author", "[\\x21-\\x21\\x23-\\x7E]{1,60}"},
    {"Keyword", "[\\x21-\\x21\\x23-\\x7E]{1,60}"},
    {"Quantity", "[1-9][0-9]{0,9}"},
    {"Price", "(:?[1-9][0-9]{0,9}(:?[.][0-9]{2,2})?|[1-9][0-9]{0,10}(:?[.][0-9]{1,1})?|0(:?[.][0-9]{1,2})?)"},
    {"Count", "(:?0|[1-9][0-9]{0,9})"}
};

// 用于匹配命令
class CommandMatch {
  private:
    std::smatch match;
    std::vector<std::pair<std::regex, std::function<void()>>> cmds;
    void EraseSpace(std::string &str) { // 去除首尾若干空格
        int k = 0;
        while (k < str.size() && str[k] == ' ') ++k;
        str.erase(0, k);
    }
  public:
    CommandMatch() = default;
    ~CommandMatch() = default;
    void AddCommand(const std::string &cmd, std::function<void()> func) {
        cmds.push_back(make_pair(std::regex(" *" + cmd + " *"), func));
    }
    bool Match(const std::string &input, std::vector<std::string> &res,
               bool UseStrSplit = 1) {
        for (const auto &it : cmds) {
            if (std::regex_match(input, match, it.first)) {
                // res.clear();
                // if (UseStrSplit) {
                //     SplitString(input, res);
                // } else for (int i = 0; i < match.size(); ++i) {
                //     res.push_back(match[i]);
                //     EraseSpace(res[i]);
                //     std::cerr << ">>> " << res[i] << std::endl;
                // }
                // it.second();
                return true;
            }
        }
        return false;
    }

};


class CommandManager {
  private:
    std::string input;
    std::vector<std::string> argv;
    int count_line = 0;
    bool DEBUG_FLAG = false;

    BookSystem Books;
    UserSystem Users;
    LogSystem  Logs;

    CommandMatch cmd_match;

    std::string LogUser() {
        return "[" + std::string(Users.GetCurrentUser().userid) + "]";
    }

    Command_t getCommand() {
        auto it = commandMap.find(argv[0]);
        if (it != commandMap.end()) {
            if (argv[0] == "show") {
                if (argv.size() >= 2 && argv[1] == "finance") {
                    return Command_t::show_finance;
                } else return Command_t::show;
            }
            return it->second;
        } else {
            if (argv[0] == "report") {
                if (argv.size() == 2) {
                    if (argv[1] == "finance") return Command_t::report_finance;
                    else if (argv[1] == "employee") return Command_t::report_employee;
                    else return Command_t::Undefined;
                }
            }
            return Command_t::Undefined;
        }
    }

    void cExit() {
        if (argv.size() != 1) throw CommandError("exit: too many arguments");
        while (!Users.stack.empty()) Users.Logout();
        Logs.log("exit success");
        throw Exit();
    }

    // {0} `su [UserID] ([Password])?`
    void cLogin() {
        if (argv.size() < 2
            || argv.size() > 3) throw CommandError("su: invalid arguments");
        if (!isValidUserID(argv[1])) throw CommandError("su: invalid UserID");
        if (argv.size() == 3
            && !isValidPassword(argv[2])) throw CommandError("su: invalid Password");
        if (argv.size() == 2) {
            Users.Login(argv[1], "");
        } else {
            Users.Login(argv[1], argv[2]);
        }
        Logs.log("login " + argv[1]);
    }

    // {1} `logout`
    void cLogout() {
        if (Users.GetCurrentUserPrivilege() < 1) throw
            CommandError("logout: Permission denied");
        if (argv.size() != 1) throw CommandError("logout: too many arguments");
        if (Users.stack.empty()) throw UserSystemError("logout: No user login");
        Logs.log("logout " + std::string(Users.GetCurrentUser().userid));
        Users.Logout();
    }

    // {0} `register [UserID] [Password] [Username]`
    void cRegist() {
        if (argv.size() != 4) throw CommandError("register: invalid arguments");
        if (!isValidUserID(argv[1])) throw CommandError("register: invalid UserID");
        if (!isValidPassword(argv[2])) throw CommandError("register: invalid Password");
        if (!isValidUsername(argv[3])) throw CommandError("register: invalid Username");
        Users.AddUser(User(argv[1], argv[3], argv[2], eCustomer));
        Logs.log(LogUser() + " register UserID=" + argv[1] + " Password=" + argv[2] +
                 " Username=" + argv[3]);
    }

    // {1} `passwd [UserID] ([CurrentPassword])? [NewPassword]`
    void cPasswd() {
        if (argv.size() < 3
            || argv.size() > 4) throw CommandError("passwd: invalid arguments");
        if (!isValidUserID(argv[1])) throw CommandError("passwd: invalid UserID");
        if (argv.size() == 4
            && !isValidPassword(argv[3])) throw CommandError("passwd: invalid Password");
        if (Users.GetCurrentUserPrivilege() < eCustomer) throw
            CommandError("passwd: Permission denied");
        if (argv.size() == 3) {
            Users.ChangePassword(argv[1], argv[2], "");
        } else if (argv.size() == 4) {
            Users.ChangePassword(argv[1], argv[3], argv[2]);
        } else throw std::runtime_error("passwd: ??");
        Logs.log(LogUser() + " passwd UserID=" + argv[1] + " NewPassword=" +
                 (argv.size() == 3 ? argv[2] : argv[3]));
    }

    // {3} `useradd [UserID] [Password] [Privilege] [Username]`
    void cUseradd() {
        if (argv.size() != 5) throw CommandError("useradd: invalid arguments");
        if (!isValidUserID(argv[1])) throw CommandError("useradd: invalid UserID");
        if (!isValidPassword(argv[2])) throw CommandError("useradd: invalid Password");
        if (!isValidPrivilege(argv[3])) throw
            CommandError("useradd: invalid Privilege");
        if (!isValidUsername(argv[4])) throw CommandError("useradd: invalid Username");
        if (Users.GetCurrentUserPrivilege() < eAdmin) throw
            CommandError("useradd: Permission denied");
        Users.UserAdd(argv[1], argv[2], acm::Privilege_t(std::stoi(argv[3])),
                      argv[4]);
        Logs.log(LogUser() + " useradd UserID=" + argv[1] + " Password=" + argv[2] +
                 " Privilege=" + argv[3] + " Username=" + argv[4]);
        if (Users.GetCurrentUserPrivilege() == eAdmin) {
            Logs.work(LogUser() + " useradd UserID=" + argv[1] + " Password=" + argv[2] +
                      " Privilege=" + argv[3] + " Username=" + argv[4]);
        }
    }

    // {7} `delete [UserID]`
    void cDelete() {
        if (argv.size() != 2) throw CommandError("delete: invalid arguments");
        if (!isValidUserID(argv[1])) throw CommandError("delete: invalid UserID");
        if (Users.GetCurrentUserPrivilege() < eRoot) throw
            CommandError("delete: Permission denied");
        Users.UserDelete(argv[1]);
        Logs.log(LogUser() + " delete UserID=" + argv[1]);
    }

    // {1} `show (-ISBN=[ISBN] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]")?`
    void cShow() {
        if (Users.GetCurrentUserPrivilege() < eCustomer) throw
            CommandError("show: Permission denied");
        if (argv.size() == 1) {
            Books.Show(sAll, "");
            Logs.log(LogUser() + " show all");
        } else if (argv.size() == 2) {
            std::string tmp;
            if (getArgument(argv[1], "-ISBN=", tmp)) {
                if (isValidISBN(tmp)) {
                    Books.Show(sISBN, tmp);
                    Logs.log(LogUser() + " show ISBN=" + tmp);
                } else throw CommandError("show: invalid ISBN");
            } else if (getArgument(argv[1], "-name=", tmp, '\"')) {
                if (isValidBookName(tmp)) {
                    Books.Show(sName, tmp);
                    Logs.log(LogUser() + " show name=\"" + tmp + "\"");
                } else throw CommandError("show: invalid BookName");
            } else if (getArgument(argv[1], "-author=", tmp, '\"')) {
                if (isValidAuthor(tmp)) {
                    Books.Show(sAuthor, tmp);
                    Logs.log(LogUser() + " show author=\"" + tmp + "\"");
                } else throw CommandError("show: invalid Author");
            } else if (getArgument(argv[1], "-keyword=", tmp, '\"')) {
                if (isValidKeyword(tmp, 2)) {
                    Books.Show(sKeyword, tmp);
                    Logs.log(LogUser() + " show keyword=\"" + tmp + "\"");
                } else throw CommandError("show: invalid Keyword");
            } else throw CommandError("show: invalid arguments");
        } else throw CommandError("show: invalid arguments");
    }

    // {1} `buy [ISBN] [Quantity]`
    void cBuy() {
        if (argv.size() != 3) throw CommandError("buy: invalid arguments");
        if (!isValidISBN(argv[1])) throw CommandError("buy: invalid ISBN");
        if (!isValidQuantity(argv[2])) throw CommandError("buy: invalid Quantity");
        if (std::stoll(argv[2]) > 2147483647ll) throw CommandError("import: invalid Quantity (too large)");
        if (Users.GetCurrentUserPrivilege() < eCustomer) throw
            CommandError("buy: Permission denied");
        double cost = Books.Buy(argv[1], std::stoul(argv[2]));
        Logs.trade(cost);
        Logs.log(LogUser() + " buy ISBN=" + argv[1] + " Quantity=" + argv[2]);
        Logs.log("trade: + " + std::to_string(cost));
    }

    // {3} `select [ISBN]`
    void cSelect() {
        if (argv.size() != 2) throw CommandError("select: invalid arguments");
        if (!isValidISBN(argv[1])) throw CommandError("select: invalid ISBN");
        if (Users.GetCurrentUserPrivilege() < eAdmin) throw
            CommandError("select: Permission denied");
        Books.Select(argv[1]);
        Users.selectstack[Users.selectstack.size() - 1] = ISBN_t(argv[1]);
        Logs.log(LogUser() + " select ISBN=" + argv[1]);
    }

    // - {3} `modify (-ISBN=[ISBN] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]" | -price=[Price])+`
    void cModify() {
        if (Users.GetCurrentUserPrivilege() < eAdmin) throw
            CommandError("modify: Permission denied");
        ISBN_t select = Users.selectstack[Users.selectstack.size() - 1];
        if (select.empty()) throw CommandError("modify: No book selected");
        Book tmp;
        Books.Select(select);
        Books.GetSelectBook(tmp);
        std::string tmpstr;
        int flag = 0;
        for (int i = 1; i < argv.size(); ++i) {
            if (getArgument(argv[i], "-ISBN=", tmpstr)) {
                if (!isValidISBN(tmpstr)) throw CommandError("modify: invalid ISBN");
                if (flag & 1) throw CommandError("modify: repeated arguments");
                flag |= 1;
                if (select == tmpstr) throw CommandError("modify: ISBN not changed");
                tmp.ISBN = tmpstr;
            } else if (getArgument(argv[i], "-name=", tmpstr, '\"')) {
                if (!isValidBookName(tmpstr)) throw CommandError("modify: invalid BookName");
                if (flag & 2) throw CommandError("modify: repeated arguments");
                flag |= 2;
                tmp.name = tmpstr;
            } else if (getArgument(argv[i], "-author=", tmpstr, '\"')) {
                if (!isValidAuthor(tmpstr)) throw CommandError("modify: invalid Author");
                if (flag & 4) throw CommandError("modify: repeated arguments");
                flag |= 4;
                tmp.author = tmpstr;
            } else if (getArgument(argv[i], "-keyword=", tmpstr, '\"')) {
                if (!isValidKeyword(tmpstr, 1)) throw CommandError("modify: invalid Keyword");
                if (flag & 8) throw CommandError("modify: repeated arguments");
                flag |= 8;
                tmp.keyword = tmpstr;
            } else if (getArgument(argv[i], "-price=", tmpstr)) {
                if (!isValidPrice(tmpstr)) throw CommandError("modify: invalid Price");
                if (flag & 16) throw CommandError("modify: repeated arguments");
                flag |= 16;
                tmp.price = std::stod(tmpstr);
            } else throw CommandError("modify: unknown argument");
        }
        if (flag == 0) throw CommandError("modify: invalid arguments, empty modify");
        Books.Modify(tmp);
        if (!tmp.ISBN.empty()) { // fuckit, 把 ISBN 改了导致用户登陆栈里 select 可能错位
            for (int i = 0; i < Users.selectstack.size(); ++i) {
                if (Users.selectstack[i] == select) Users.selectstack[i] = tmp.ISBN;
            }
        }
        Logs.log(LogUser() + " modify " + std::string(select)
                 + ((flag & 1) ? " -ISBN=" + std::string(tmp.ISBN) : "")
                 + ((flag & 2) ? " -name=\"" + std::string(tmp.name) + "\"" : "")
                 + ((flag & 4) ? " -author=\"" + std::string(tmp.author) + "\"" : "")
                 + ((flag & 8) ? " -keyword=\"" + std::string(tmp.keyword) + "\"" : "")
                 + ((flag & 16) ? " -price=" + std::to_string(tmp.price) : ""));
        if (Users.GetCurrentUserPrivilege() == eAdmin) {
            Logs.work(LogUser() + " modify " + std::string(select)
                      + ((flag & 1) ? " -ISBN=" + std::string(tmp.ISBN) : "")
                      + ((flag & 2) ? " -name=\"" + std::string(tmp.name) + "\"" : "")
                      + ((flag & 4) ? " -author=\"" + std::string(tmp.author) + "\"" : "")
                      + ((flag & 8) ? " -keyword=\"" + std::string(tmp.keyword) + "\"" : "")
                      + ((flag & 16) ? " -price=" + std::to_string(tmp.price) : ""));
        }
    }

    // {3} `import [Quantity] [TotalCost]`
    void cImport() {
        if (argv.size() != 3) throw CommandError("import: invalid arguments");
        if (!isValidQuantity(argv[1])) throw CommandError("import: invalid Quantity");
        if (std::stoll(argv[1]) > 2147483647ll) throw CommandError("import: invalid Quantity (too large)");
        if (!isValidPrice(argv[2])) throw CommandError("import: invalid TotalCost");
        if (Users.GetCurrentUserPrivilege() < eAdmin) throw
            CommandError("import: Permission denied");
        ISBN_t select = Users.selectstack[Users.selectstack.size() - 1];
        if (select.empty()) throw CommandError("modify: No book selected");
        Books.Select(select);
        Books.Import(std::stoul(argv[1]));
        Logs.trade(-1.0 * std::stod(argv[2]));
        Logs.log(LogUser() + " import " + std::string(select) + " Quantity=" + argv[1] +
                 " TotalCost=" + argv[2]);
        Logs.log("trade: - " + std::to_string(Abs(std::stod(argv[2]))));
        if (Users.GetCurrentUserPrivilege() == eAdmin) {
            Logs.work(LogUser() + " import " + std::string(select) + " Quantity=" + argv[1]
                      + " TotalCost=" + argv[2]);
        }
    }

    // {7} `show finance ([Count])?`
    void cShowFinance() {
        if (argv.size() < 2
            || argv.size() > 3) throw CommandError("show finance: invalid arguments");
        if (Users.GetCurrentUserPrivilege() < eRoot) throw
            CommandError("show finance: Permission denied");
        if (argv.size() == 2) {
            Logs.showFinance();
            Logs.log(LogUser() + " show finance");
        } else if (argv.size() == 3) {
            if (!isValidCount(argv[2])) throw CommandError("show finance: invalid Count");
            Logs.showFinance(std::stoi(argv[2]));
            Logs.log(LogUser() + " show finance " + argv[2]);
        }
    }

    // {7} `log`🎗️
    void cLog() {
        if (argv.size() != 1) throw CommandError("log: invalid arguments");
        if (Users.GetCurrentUserPrivilege() < eRoot) throw
            CommandError("log: Permission denied");
        Logs.showLog();
    }

    // {7} `report finance` 🎗️
    void cReportFinance() {
        if (argv.size() != 2) throw CommandError("report finance: invalid arguments");
        if (Users.GetCurrentUserPrivilege() < eRoot) throw
            CommandError("report finance: Permission denied");
        Logs.showTrade();
    }

    // {7} `report employee` 🎗️
    void cReportEmployee() {
        if (argv.size() != 2) throw CommandError("report finance: invalid arguments");
        if (Users.GetCurrentUserPrivilege() < eRoot) throw
            CommandError("report finance: Permission denied");
        Logs.showWork();
    }

    void cDebug() {
        if (!DEBUG_FLAG) throw
            CommandError("debug: Permission denied. (You should enable debug mode when start)");
        std::cerr << "================== DEBUG ==================" << std::endl;
        Users.Debug();
        Books.Debug();
        std::cerr << "\nUser stack:\n" << std::endl;
        for (int i = 0; i < Users.stack.size(); ++i) {
            std::cerr << std::string(Users.stack[i].userid) << " " << Users.selectstack[i]
                      << std::endl;
        }
        std::cerr << "================ END DEBUG ================" << std::endl;
    }

  public:
    CommandManager(bool debug = 0) {
        Logs.log(std::string("start.") + (debug ? " with debug mode" : ""));
        DEBUG_FLAG = debug;
        count_line = 0;
        cmd_match.AddCommand(format("su +(%s)( +%s)?", reg["UserID"],
        reg["Password"]), [this]() {cLogin(); });
        cmd_match.AddCommand(format("quit|exit"), [this]() {cExit();});
        cmd_match.AddCommand(format("logout"), [this]() {cLogout();});
        cmd_match.AddCommand(format("register +(%s) +(%s) +(%s)", reg["UserID"],
        reg["Password"], reg["Username"]), [this]() {cRegist();});
        cmd_match.AddCommand(format("passwd +(%s)( +%s)? +(%s)", reg["UserID"],
        reg["Password"], reg["Password"]), [this]() {cPasswd();});
        cmd_match.AddCommand(format("useradd +(%s) +(%s) +(%s) +(%s)", reg["UserID"],
        reg["Password"], reg["Privilege"], reg["Username"]), [this]() {cUseradd();});
        cmd_match.AddCommand(format("delete +(%s)", reg["UserID"]), [this]() {cDelete();});
        cmd_match.AddCommand(
            format("show( +-ISBN=%s| +-name=\"%s\"| +-author=\"%s\"| +-keyword=\"%s\")?",
        reg["ISBN"], reg["BookName"], reg["Author"], reg["Keyword"]), [this]() {cShow();});
        cmd_match.AddCommand(format("buy +(%s) +(%s)", reg["ISBN"],
        reg["Quantity"]), [this]() {cBuy();});
        cmd_match.AddCommand(format("select +(%s)", reg["ISBN"]), [this]() {cSelect();});
        cmd_match.AddCommand(
            format("modify( +-ISBN=%s| +-name=\"%s\"| +-author=\"%s\"| +-keyword=\"%s\"| +-price=%s)+",
                   reg["ISBN"], reg["BookName"], reg["Author"], reg["Keyword"],
        reg["Price"]), [this]() {cModify();});
        cmd_match.AddCommand(format("import +(%s) +(%s)", reg["Quantity"],
        reg["Price"]), [this]() {cImport();});
        cmd_match.AddCommand(format("show finance( +(%s))?",
        reg["Count"]), [this]() {cShowFinance();});
        cmd_match.AddCommand(format("log"), [this]() {cLog();});
        cmd_match.AddCommand(format("report +finance"), [this]() {cReportFinance();});
        cmd_match.AddCommand(format("report +employee"), [this]() {cReportEmployee();});
        cmd_match.AddCommand(format("debug"), [this]() {cDebug();});
        cmd_match.AddCommand(format(" *"), []() {});
    }
    ~CommandManager() = default;

    void NextCommand() {
        std::getline(std::cin, input);
        ++count_line;
        if (std::cin.eof()) {
            while (!Users.stack.empty()) Users.Logout();
            Logs.log("exit success");
            throw Exit();
        }
        if (!cmd_match.Match(input, argv)) {
            throw std::runtime_error("Invalid command");
        }
        SplitString(input, argv);
        if (DEBUG_FLAG) {
            std::cerr << count_line << ":Input: " << input << ";  >>> ";
            std::cerr << "{ ";
            for (int i = 0; i < argv.size(); ++i) {
                std::cerr << "[" << i << "]=" << argv[i] << ",";
            }
            std::cerr << " }" << std::endl;
        }
        if (argv.size() == 0) return ;
        Command_t cmd = getCommand();
        switch (cmd) {
            case Command_t::quit:
            case Command_t::exit: cExit(); break;
            case Command_t::su: cLogin(); break;
            case Command_t::logout: cLogout(); break;
            case Command_t::regist: cRegist(); break;
            case Command_t::passwd: cPasswd(); break;
            case Command_t::useradd: cUseradd(); break;
            case Command_t::del: cDelete(); break;
            case Command_t::show: cShow(); break;
            case Command_t::buy: cBuy(); break;
            case Command_t::select: cSelect(); break;
            case Command_t::modify: cModify(); break;
            case Command_t::import: cImport(); break;
            case Command_t::show_finance: cShowFinance(); break;
            case Command_t::log: cLog(); break;
            case Command_t::report_finance: cReportFinance(); break;
            case Command_t::report_employee: cReportEmployee(); break;
            case Command_t::debug: cDebug(); break;
            case Command_t::Undefined: throw CommandError("Unknown command");
        }
    }

    void Run() {
        while (true) {
            try {
                NextCommand();
            } catch (const Exit &e) {
                if (DEBUG_FLAG) std::cerr << "Exit Program" << std::endl;
                break;
            } catch (const Exception &e) {
                if (DEBUG_FLAG) std::cerr << count_line << ":Input: " << input << "  >>> " <<
                                              e.what() << std::endl;
                std::cout << "Invalid\n";
            } catch (const std::exception &e) {
                if (DEBUG_FLAG) std::cerr << count_line << ":Input: " << input << "  >>> " << e.what() << std::endl;
                std::cout << "Invalid\n";
                // break;
            }
        }
    }
};

}


#endif // COMMAND_HPP