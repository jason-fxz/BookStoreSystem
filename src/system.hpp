// 主要的信息维护实现

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "exception.hpp"
#include "utility.hpp"
#include "book.hpp"
#include "user.hpp"
#include "loger.hpp"
#include <ios>
#include <vector>

namespace acm {

class UserSystem : private UserDataBase {
    friend class CommandManager;
  private:
    std::vector<User> stack; // 登录栈
    std::vector<ISBN_t> selectstack; // 对应登录栈中人的当前选择的书
    std::map<UserID_t, int> instack;

    const User EmptyUser = User();

    const User &GetCurrentUser() const {
        if (stack.empty()) return EmptyUser;
        return stack.back();
    }
    Privilege_t GetCurrentUserPrivilege() const {
        if (stack.empty()) return eGuest;
        return stack.back().privilege;
    }
    void Push(const User &user) {
        stack.push_back(user);
        selectstack.push_back(ISBN_t());
        if (instack.find(user.userid) == instack.end()) {
            instack[user.userid] = 1;
        } else {
            instack[user.userid]++;
        }
    }
    void Pop() {
        instack[stack.back().userid]--;
        stack.pop_back();
        selectstack.pop_back();
    }
  public:
    ~UserSystem() = default;
    UserSystem() = default;

    // 登录帐户
    void Login(const UserID_t &userid, const Password_t &password) {
        User tmpuser;
        GetUser(userid, tmpuser);
        if (password.empty()) {
            if (GetCurrentUserPrivilege() <= tmpuser.privilege) throw
                UserSystemError("Login: you should enter password");
        }
        if (tmpuser.password != password) {
            throw UserSystemError("Login: wrong password");
        }
        Push(tmpuser);
    }

    // 注销帐户
    void Logout() {
        if (stack.empty()) throw UserSystemError("Logout: No user login");
        Pop();
    }

    // 注册帐户
    void Register(const UserID_t &userid, const Password_t &password,
                  const UserName_t &username) {
        AddUser(User(userid, password, username, eCustomer));
    }

    // 修改密码
    void ChangePassword(const UserID_t &userid, const Password_t &password_new,
                        const Password_t &password_cur) {
        // if (GetCurrentUserPrivilege() < eCustomer) throw std::runtime_error("Permission denied");
        if (password_cur.empty()
            && GetCurrentUserPrivilege() < eRoot) throw
            UserSystemError("you should enter current password");
        User tmpuser; GetUser(userid, tmpuser);
        if (!password_cur.empty()
            && password_cur != tmpuser.password) throw
            UserSystemError("wrong current password");
        tmpuser.password = password_new;
        ModifyUser(tmpuser);
    }

    // 创建帐户
    void UserAdd(const UserID_t &userid, const Password_t &password,
                 const Privilege_t &privilege, const UserName_t &username) {
        // if (GetCurrentUserPrivilege() < eAdmin) throw std::runtime_error("Permission denied");
        if (privilege >= GetCurrentUserPrivilege()) throw
            UserSystemError("Permission denied: you can't create a user with higher privilege than yours");
        AddUser(User(userid, username, password, privilege));
    }

    // 删除帐户
    void UserDelete(const UserID_t &userid) {
        // if (GetCurrentUserPrivilege() < eRoot) throw std::runtime_error("Permission denied");
        if (instack[userid] > 0) throw UserSystemError("UserDelete: User is online");
        DeleteUser(userid);
    }

    void Debug() {
        ShowAllUser();
    }

};


class BookSystem : private BookDataBase {

  public:
    BookSystem() = default;
    ~BookSystem() = default;

    // 输出满足要求的书籍
    template <class words_t>
    void Show(const Search_t &type, const words_t &words = words_t()) {
        std::vector<Book> res;
        switch (type) {
        case sAll:
            ShowAllBook(res);
            break;
        case sISBN:
            SearchBookByISBN(words, res);
            break;
        case sName:
            SearchBookByName(words, res);
            break;
        case sAuthor:
            SearchBookByAuthor(words, res);
            break;
        case sKeyword:
            SearchBookByKeyword(words, res);
            break;
        default:
            throw BookSystemError("Show: Invalid SearchType");
        }
        bool emptyflag = 1;
        for (auto &book : res) if (!book.ISBN.empty()) {
            // [ISBN]\t[BookName]\t[Author]\t[Keyword]\t[Price]\t[库存数量]\n
            emptyflag = 0;
            std::cout << book.ISBN << "\t" << book.name << "\t" << book.author << "\t" <<
                      book.keyword << "\t" << std::fixed << std::setprecision(2) << book.price << "\t" << book.quantity << "\n";
        }
        if (emptyflag) std::cout << "\n";
    }

    // 购买图书
    double Buy(const ISBN_t &ISBN, const int &quantity) {
        std::vector<Book> tmp;
        SearchBookByISBN(ISBN, tmp);
        if (tmp.empty()) throw BookSystemError("Buy: No such books");
        if (tmp[0].quantity < quantity) throw BookSystemError("Buy: No enough books");
        double totcost = tmp[0].price * quantity;
        tmp[0].quantity -= quantity;
        SelectByISBN(tmp[0].ISBN);
        ModifyBook(tmp[0]);
        std::cout << std::fixed << std::setprecision(2) << totcost << "\n";
        return totcost;
    }

    // 根据 ISBN 选择一本书，如果没有则创建
    void Select(const ISBN_t &ISBN) {
        SelectByISBN(ISBN);
    }

    void GetSelectBook(Book &res) {
        GetCache(res);
    }

    // 修改图书信息
    void Modify(const Book &obj) {
        ModifyBook(obj);
    }

    // 图书进货
    void Import(const int &quantity) {
        Book tmp;
        GetCache(tmp);
        tmp.quantity += quantity;
        Modify(tmp);
    }

    void Debug() {
        std::cerr << "All Books:" << std::endl;
        std::vector<Book> tmp;
        ShowAllBook(tmp);
        for (auto &book : tmp) {
            std::cerr << book << std::endl;
        }
    }

};

class LogSystem {
  private:
    Loger logInfo;    // 记录日志
    Loger workInfo;   // 记录员工
    Loger tradeInfo;  // 记录收支
    char tmpstr[256];
  public:
    LogSystem() : workInfo("./work.log"), logInfo("./log.log"),
        tradeInfo("./trade.log") {}
    ~LogSystem() {}

    void log(const std::string &str, const char &end = '\n') {
        logInfo.write(str + end);
    }

    void trade(const double &money) {
        sprintf(tmpstr, "%c %13.2lf\n", money >= 0 ? '+' : '-', Abs(money));
        tradeInfo.write(tmpstr);
    }

    void work(const std::string &str, const char &end = '\n') {
        workInfo.write(str + end);
    }

    void showLog() {
        std::cout << "================ logInfo ================\n";
        logInfo.printAll();
        std::cout << "================ tradeInfo ================\n";
        tradeInfo.printAll();
    }

    void showWork() {
        std::cout << "================ workInfo ================\n";
        workInfo.printAll();
    }

    void showTrade() {
        std::cout << "================ tradeInfo ================\n";
        tradeInfo.printAll();
    }

    void showFinance(const int &count = -1) {
        tradeInfo.flush();
        if (count == -1) { // show all
            tradeInfo.file.seekg(0, std::ios::beg);
        } else {
            tradeInfo.file.seekg(- count * 16, std::ios::end);
            if (!tradeInfo.file.good()) throw LogSystemError("showFinance: seekg error");
        }
        double income = 0, outcome = 0;
        char ch;
        while (tradeInfo.file >> ch >> tmpstr) {
            if (ch == '+') {
                income += atof(tmpstr);
            } else {
                outcome += atof(tmpstr);
            }
        }
        if (income == 0 && outcome == 0) {
            std::cout << "\n";
        } else {
            std::cout << "+ " << income << " - " << outcome << "\n";
        }
        tradeInfo.file.clear();
    }
};

}



#endif // SYSTEM_HPP