// 书本信息管理

#ifndef BOOKS_HPP
#define BOOKS_HPP

#include "utility.hpp"
#include "database.hpp"
#include <cstddef>

namespace acm {


// 定义书本类
struct Book {
    ISBN_t     ISBN;
    BookName_t name;
    Author_t   author;
    Keyword_t  keyword;

    double     price;
    size_t        quantity;

    Book() : ISBN(), name(), author(), keyword(), price(0), quantity(0) {}

    Book(const ISBN_t &_ISBN) :
        ISBN(_ISBN),
        name(),
        author(),
        keyword(),
        price(0),
        quantity(0) {}

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
        os << "ISBN=" << obj.ISBN << ",BookName=\"" << obj.name << "\",Author=\"" << obj.author << "\",Keyword=\"" << obj.keyword
           << "\",Price=" << std::fixed << std::setprecision(2) << obj.price << ",Quantity=" << obj.quantity;
        return os;
    }

    friend istream &operator>>(istream &is, Book &obj) {
        is >> obj.ISBN >> obj.name >> obj.author >> obj.keyword >> obj.price >>
           obj.quantity;
        return is;
    }
};


// 书本数据库
class BookDataBase {
  private:
    database<ISBN_t, Book>       lISBN;
    database<BookName_t, ISBN_t> lBookName;
    database<Author_t, ISBN_t>   lAuthor;
    database<Keyword_t, ISBN_t>  lKeyword;

    ISBN_t cur; // 当前操作的书本 ISBN
    Book cache; // 缓存当前操作的书本信息

  public:
    BookDataBase() :
        lISBN("./db1.bin", "./db2.bin"),
        lBookName("./db3.bin", "./db4.bin"),
        lAuthor("./db5.bin", "./db6.bin"),
        lKeyword("./db7.bin", "./db8.bin") {}

    ~BookDataBase() = default;

    void AddBook(const Book &obj) {
        lISBN.insert(obj.ISBN, obj);
        if (!obj.name.empty()) lBookName.insert(obj.name, obj.ISBN);
        if (!obj.author.empty()) lAuthor.insert(obj.author, obj.ISBN);
        if (!obj.ISBN.empty()) lKeyword.insert(obj.keyword, obj.ISBN);
    }

    void SearchBookByISBN(const ISBN_t &ISBN, std::vector<Book> &res) {
        lISBN.fetchall(ISBN, res);
    }

    void SearchBookByName(const BookName_t &name, std::vector<Book> &res) {
        std::vector<ISBN_t> tmp;
        lBookName.fetchall(name, tmp);
        for (auto &ISBN : tmp)
            lISBN.fetchall(ISBN, res);
    }

    void SearchBookByAuthor(const Author_t &author, std::vector<Book> &res) {
        std::vector<ISBN_t> tmp;
        lAuthor.fetchall(author, tmp);
        for (auto &ISBN : tmp)
            lISBN.fetchall(ISBN, res);
    }

    void SearchBookByKeywords(const Keyword_t &keywords, std::vector<Book> &res) {
        std::vector<Keyword_t> tmpkeywords;
        std::vector<ISBN_t> tmp;
        SplitString(keywords, tmpkeywords, '|');
        for (auto &key : tmpkeywords)
            lKeyword.fetchall(key, tmp);
        for (auto &ISBN : tmp)
            lISBN.fetchall(ISBN, res);
    }

    void SearchBookByKeyword(const Keyword_t &keyword, std::vector<Book> &res) {
        std::vector<ISBN_t> tmp;
        lKeyword.fetchall(keyword, tmp);
        for (auto &ISBN : tmp)
            lISBN.fetchall(ISBN, res);
    }

    void SearchAll(std::vector<Book> &res) {
        lISBN.fetchall("", res);
    }

    void ShowAllBook() {
        lISBN.ShowAll();
    }

    // 根据 ISBN 选择一本书，如果没有则创建
    void SelectByISBN(const ISBN_t &ISBN) {
        cur = ISBN;
        lISBN.insertkey(ISBN, Book(ISBN));
        std::vector<Book> booklist;
        lISBN.fetchall(cur, booklist);
        if (booklist.size() != 1) throw std::runtime_error("SelectByISBN: booklist.size() != 1");
        cache = booklist[0];

    }

    // 修改 cur 对应书本信息， 使用前一定要 SelectByISBN
    void ModifyBook(const Book &obj) {
        if (cur.empty()) throw std::runtime_error("No selected book");
        if (obj.ISBN != cur) {
            if (!lISBN.insertkey(obj.ISBN, obj)) throw BookSystemError("ModifyBook: ISBN already exists");
            lISBN.removekey(cur);
        } else lISBN.modify(cur, obj);
        if (Camp(obj.name, cache.name) != 0) {
            lBookName.remove(cache.name, obj.ISBN);
            lBookName.insert(obj.name, obj.ISBN);
        }
        if (Camp(obj.author, cache.author) != 0) {
            lAuthor.remove(cache.author, obj.ISBN);
            lAuthor.insert(obj.author, obj.ISBN);
        }
        if (Camp(obj.keyword, cache.keyword) != 0) {
            std::vector<Keyword_t> keywords;
            SplitString(cache.keyword, keywords, '|');
            for (auto &key : keywords)
                lKeyword.remove(key, obj.ISBN);
            SplitString(obj.keyword, keywords, '|');
            for (auto &key : keywords)
                lKeyword.insert(key, obj.ISBN);
        }
    }

    void GetCache(Book &res) {
        res = cache;
    }

};

}

#endif // BOOKS_HPP