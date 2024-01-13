// 书本信息管理

#ifndef BOOKS_HPP
#define BOOKS_HPP

#include "utility.hpp"

namespace acm {


// 定义书本类
struct Book {
    ISBN_t     ISBN;
    BookName_t name;
    Author_t   author;
    Keyword_t  keyword;

    double     price;
    int        quantity;

    Book() : ISBN(), name(), author(), keyword(), price(0), quantity(0) {}

    Book(const ISBN_t &_ISBN,
         const BookName_t &_name,
         const Author_t &_author,
         const Keyword_t &_keyword,
         const double &_price,
         const int &_quantity) :
        ISBN(_ISBN),
        name(_name),
        author(_author),
        keyword(_keyword),
        price(_price),
        quantity(_quantity) {}

    Book(const Book &rhs) :
        ISBN(rhs.ISBN),
        name(rhs.name),
        author(rhs.author),
        keyword(rhs.keyword),
        price(rhs.price),
        quantity(rhs.quantity) {}

    ~Book() = default;

    Book &operator=(const Book &rhs) {
        if (this != &rhs) {
            ISBN = rhs.ISBN;
            name = rhs.name;
            author = rhs.author;
            keyword = rhs.keyword;
            price = rhs.price;
            quantity = rhs.quantity;
        }
        return *this;
    }

    bool operator<(const Book &rhs) const {
        return Camp(ISBN, rhs.ISBN) < 0;
    }

    bool operator==(const Book &rhs) const {
        return Camp(ISBN, rhs.ISBN) == 0;
    }

    friend ostream &operator<<(ostream &os, const Book &obj) {
        os << obj.ISBN << " " << obj.name << " " << obj.author << " " << obj.keyword <<
           " " << obj.price << " "
           << obj.quantity;
        return os;
    }

    friend istream &operator>>(istream &is, Book &obj) {
        is >> obj.ISBN >> obj.name >> obj.author >> obj.keyword >> obj.price >>
           obj.quantity;
        return is;
    }
};

}

#endif // BOOKS_HPP