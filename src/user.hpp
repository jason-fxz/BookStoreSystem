// 用户信息管理

#ifndef USER_HPP
#define USER_HPP

#include "utility.hpp"
#include "database.hpp"

namespace acm {

// 定义用户类
struct User {
    UserID_t    userid;
    UserName_t  username;
    Password_t  password;

    Privilege_t privilege;

    User() : userid(), username(), password(), privilege(eGuest) {}

    User(const UserID_t &_userid,
         const UserName_t &_username,
         const Password_t &_password,
         const Privilege_t &_privilege) : userid(_userid), username(_username),
        password(_password), privilege(_privilege) {}

    User(const User &rhs) : userid(rhs.userid), username(rhs.username),
        password(rhs.password), privilege(rhs.privilege) {}

    ~User() = default;

    User &operator=(const User &rhs) {
        if (this != &rhs) {
            userid = rhs.userid;
            username = rhs.username;
            password = rhs.password;
            privilege = rhs.privilege;
        }
        return *this;
    }

    bool operator<(const User &rhs) const {
        return Camp(userid, rhs.userid) < 0;
    }

    bool operator==(const User &rhs) const {
        return Camp(userid, rhs.userid) == 0;
    }
    
    friend ostream &operator<<(ostream &os, const User &rhs) {
        os << rhs.userid << ',' << rhs.username << ',' << rhs.password << ','
           << rhs.privilege;
        return os;
    }

    // friend istream &operator>>(istream &is, User &rhs) {
    //     is >> rhs.userid >> rhs.username >> rhs.password >> rhs.privilege;
    //     return is;
    // }

};


// 用户数据库
class UserDataBase {
private:
    database<UserID_t, User> lUserID;

public:
    UserDataBase() :
        lUserID("./data/db9.bin", "./data/db10.bin") {
            if (lUserID.empty()) {
                AddUser(User("root", "root", "sjtu", eRoot));
            }
        }

    ~UserDataBase() = default;

    void AddUser(const User &obj) {
        std::cerr << "AddUser " << obj << std::endl;
        if (!lUserID.insertkey(obj.userid, obj)) throw std::runtime_error("AddUser: UserID already exists");
    }

    void GetUser(const UserID_t &userid, User &user) {
        std::vector<User> tmp;
        lUserID.fetchall(userid, tmp);
        if (tmp.empty()) throw std::runtime_error("GetUser: fetchall failed");
        user = tmp[0];
    }

    void DeleteUser(const UserID_t &userid) {
        if (!lUserID.removekey(userid)) throw std::runtime_error("DeleteUser: no such user");
    }

    void ModifyUser(const User &obj) {
        if (!lUserID.modify(obj.userid, obj)) throw std::runtime_error("ModifyUser: modify failed");
    }

    void ShowAllUser() {
        std::cerr << "All Users:" << std::endl;
        lUserID.ShowAll();
    }
};


}



#endif // USER_HPP