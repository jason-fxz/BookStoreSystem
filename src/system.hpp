// 主要的信息维护实现

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "utility.hpp"
#include "book.hpp"
#include "user.hpp"
#include "loger.hpp"
#include <vector>

namespace acm {

class UserSystem : private UserDataBase {
friend class BookSystem;
private:
    std::vector<User> stack; // 登录栈
    std::vector<ISBN_t> selectstack; // 对应登录栈中人的当前选择的书
    std::map<UserID_t, int> instack;

    User GetCurrentUser() const {
        return stack.back();
    }
    Privilege_t GetCurrentUserPrivilege() const {
        if (stack.empty()) return eGuest;
        return stack.back().privilege;
    }
    void Push(const User &user) {
        stack.push_back(user);
        if (instack.find(user.userid) == instack.end()) {
            instack[user.userid] = 1;
        } else {
            instack[user.userid]++;
        }

    }
    void Pop() {
        instack[stack.back().userid]--;
        stack.pop_back();
    }
public:
    ~UserSystem() = default;
    UserSystem() = default;

    // 登录帐户
    void Login(const UserID_t &userid, const Password_t &password) {
        User tmpuser;
        GetUser(userid, tmpuser);
        if (password.empty()) {
            if (GetCurrentUserPrivilege() <= tmpuser.privilege) throw std::runtime_error("you should enter password");
        }
        if (tmpuser.password != password) {
            throw std::runtime_error("Invalid password");
        }
        Push(tmpuser);
    }

    // 注销帐户
    void Logout() {
        if (stack.empty()) throw std::runtime_error("No user login");
        Pop();
    }

    // 注册帐户
    void Register(const UserID_t &userid, const Password_t &password, const UserName_t &username) {
        AddUser(User(userid, password, username, eCustomer));
    }

    // 修改密码
    void ChangePassword(const UserID_t &userid, const Password_t &password_new, const Password_t &password_cur) {
        if (GetCurrentUserPrivilege() < eCustomer) throw std::runtime_error("Permission denied");
        if (password_cur.empty() && GetCurrentUserPrivilege() < eRoot) throw std::runtime_error("you should enter current password");
        User tmpuser; GetUser(userid, tmpuser);
        if (password_cur != tmpuser.password) throw std::runtime_error("Invalid current password");
        tmpuser.password = password_new;
        ModifyUser(tmpuser);
    }

    // 创建帐户
    void UserAdd(const UserID_t &userid, const Password_t &password, const Privilege_t &privilege, const UserName_t &username) {
        if (GetCurrentUserPrivilege() < eAdmin) throw std::runtime_error("Permission denied");
        if (privilege >= GetCurrentUserPrivilege()) throw std::runtime_error("Permission denied: you can't create a user with higher privilege than yours");
        AddUser(User(userid, username, password, privilege));
    }

    // 删除帐户
    void UserDelete(const UserID_t &userid) {
        if (GetCurrentUserPrivilege() < eRoot) throw std::runtime_error("Permission denied");
        if (instack[userid] > 0) throw std::runtime_error("UserDelete: User is online");
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
    void Show(const Search_t &type, const words_t & words = words_t()) {
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
                throw std::runtime_error("Show: Invalid SearchType");
        }
        // std::sort(res.begin(), res.end());
        if (res.empty()) std::cout << "\n";
        else for (auto &book : res) {
            std::cout << book << "\n";
        }
    }

    // 购买图书
    void Buy(const ISBN_t &ISBN, const int &quantity) {
        std::vector<Book> tmp;
        SearchBookByISBN(ISBN, tmp);
        if (tmp.empty()) throw std::runtime_error("Buy: No such books");
        if (tmp[0].quantity < quantity) throw std::runtime_error("Buy: No enough books");
        double totcost = tmp[0].price * quantity;
        tmp[0].quantity -= quantity;
        ModifyBook(tmp[0]);
        std::cout << std::fixed << std::setprecision(2) << totcost << "\n";
    }

    // 根据 ISBN 选择一本书，如果没有则创建
    void Select(const ISBN_t &ISBN) {
        SelectByISBN(ISBN);
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

};

class LogSystem {
  private:
    Loger logInfo;    // 记录日志
    Loger workInfo;   // 记录员工
    Loger tradeInfo;  // 记录收支
    char tmpstr[256];
  public:
    LogSystem() : workInfo("./log/work.txt"), logInfo("./log/log.txt"), tradeInfo("./log/trade.txt") {}
    ~LogSystem() {}

    void log(const std::string &str) {
        logInfo.write(str);
    }

    void trade(const double &money) {
        sprintf(tmpstr, "%c %13.2lf\n", money >= 0 ? '+' : '-', Abs(money));
        tradeInfo.write(tmpstr);
    }

    void work(const std::string &str) {
        workInfo.write(str);
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

    void showFinance(const int &count = -1) {
        tradeInfo.flush();
        if (count == -1) { // show all
            tradeInfo.file.seekg(0, std::ios::beg);
        } else {
            tradeInfo.file.seekg(- count * 16, std::ios::end);
            if (!tradeInfo.file.good()) throw std::runtime_error("showFinance: seekg error");
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