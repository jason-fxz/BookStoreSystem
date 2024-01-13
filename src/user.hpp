// 用户信息管理

#ifndef USER_HPP
#define USER_HPP

#include "utility.hpp"

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
        os << rhs.userid << '\t' << rhs.username << '\t' << rhs.password << '\t'
           << rhs.privilege;
        return os;
    }

    // friend istream &operator>>(istream &is, User &rhs) {
    //     is >> rhs.userid >> rhs.username >> rhs.password >> rhs.privilege;
    //     return is;
    // }

};



}



#endif // USER_HPP