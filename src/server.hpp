#include "exception.hpp"
#include "user.hpp"
#include "book.hpp"
#include "system.hpp"

#include "utility.hpp"

#include <jwt-cpp/jwt.h>
#include <picojson/picojson.h>
#include "./include/httplib.h"
// #include <picojson/picojson.h>
namespace acm {
using namespace httplib;
class MyWebServer {
  private:
    const std::string rsa_priv_key = R"(-----BEGIN PRIVATE KEY-----
MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCibgUYPVLQShsS
Wr/R7SUFew9iPWp3ZSlGwpC7rD7E8GF9CTouAaM8y7yWITRlXLhwKTAzZloLeiB/
+BPDz8ipVcSOXGxs4CkRZ1vfPBc/bTTP7ugyU46atHXxiEhZZpUp10lhPBGvy7Y7
Jn/BRAGHH01XhdbvliM3Nrzq5D7ABaU4aF8h0SdNoIwTdr8K7kZQrIp8PbuGdOp+
hmWOha0w5gHkjqwkuuZvYiTNxRj5gXpnDEookH5MmxDlTLghlqLJNFafeCy7su+a
4ol+eaaInj+s3TB7SeEOyI8pzXIWcOYCVNE77q3YgFsEeBdTiS3QYYsFQAJeIvHP
jjMTnvTpAgMBAAECggEAAqCM7ItxX3UTYwScvaR8SM5ovUoJAs1cEYzsEl0C1uSc
bw1LJ5tXLJCPCE6ZLlqFkSrZYLtzEWR0Sh2VFEG0GSkN+M994XTutR1a2w15wsjd
Zc+Tadz4HYB1t+ZgZTHHj9mvkicKkjFVTTeeMV4MOEdFVPkRNObUwvvbqmDusuRd
4tINVcDVGxJwAAoAHqcAxczOBYjB8dq8IHHD13BPhsFjLnKmmiN8qhEte6vDie/J
TD67213C+5YSw8bj1hdEoQmjbHn4pEOisnB3sfJVfqYKXqq3pVgDXoHNBXS5ZOXF
ma01Kc5a+qQYzTecofpdFAI8+Gg0IiftP3t7fih90QKBgQDIfHFPlxzZxF85sKFx
3vDdz9JwfX9B64FfCVj7nJX1gOjROV23d+cQf2Tl88I5g/ClJcEgnft004sIbWVm
zLYVVskPdtwF79VWsd+OegQmRf44Tq3/TUq2vpq3BMv8Ls85nIwUEH4p7/CZFnZ4
ld69tob1NTQqdF3bMjDVC+GcXwKBgQDPZ+9Le7C223qKyMACoRehTjyuMzR1HDx8
MwerWEy1A9BI3ARDsxXQu2DbJQ+fSkHNhOiaD4EmIOXs+PP4OBzBigW7idfGnmUX
7RPajhzCrfexOD8OYzYOp85SC3eYSl5p38rzgYh6AebV5i2cbMLRGQ2MesBcqBCV
TvBz2lbztwKBgD2psLw3ESOVd2hqAC3i0RiX+/SnVMW0ppTRbjHtRl49KN55E28D
jOkRFbp4T6ua/h/9Tbo3fQjAq8mKxB5rJjHIAI0mFKQq0OYaTVUoCDCxZHBzGqck
Ya91TmauLgNBbnEcyiS9seA6MTkUpJmQFvMoubefhozsP5XUiRptzhVnAoGAdqX7
g4/vyLuHv6sWBKsJLJLoLbNHOBN+GoLKyV+LCiZzWarbmaryOOVohBUQecxuFlcJ
BVEHTbuWLNlCH+ZhtOnzW5SAZrQoGubZ67ZOh6QjxTLrB50DyKCAa/j5YppwplqK
7IBWhY1e81BpGUoyrJGLaaizMI5IZlGxRmVKRsMCgYBqGHKc5ztBCl3rNru1Y2aD
1CBiM4yqUs+77fWH//RGTBkfhibGNTF5SQXSo6Z757iHCMKHTPxsR/YYQv/0elCD
1qoLebs94anft1R5bpXzoD1/ef+/2/Oxv5z4oCubT5z/M9EHB7ojBLfl6uz+jiWh
9sYAnVj5JhFbipPgiSi4tQ==
-----END PRIVATE KEY-----)";
    const std::string rsa_pub_key = R"(-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAom4FGD1S0EobElq/0e0l
BXsPYj1qd2UpRsKQu6w+xPBhfQk6LgGjPMu8liE0ZVy4cCkwM2ZaC3ogf/gTw8/I
qVXEjlxsbOApEWdb3zwXP200z+7oMlOOmrR18YhIWWaVKddJYTwRr8u2OyZ/wUQB
hx9NV4XW75YjNza86uQ+wAWlOGhfIdEnTaCME3a/Cu5GUKyKfD27hnTqfoZljoWt
MOYB5I6sJLrmb2IkzcUY+YF6ZwxKKJB+TJsQ5Uy4IZaiyTRWn3gsu7LvmuKJfnmm
iJ4/rN0we0nhDsiPKc1yFnDmAlTRO+6t2IBbBHgXU4kt0GGLBUACXiLxz44zE570
6QIDAQAB
-----END PUBLIC KEY-----)";

    UserSystemWeb Users;
    BookSystemWeb Books;
    LogSystemWeb  Logs;

    void VerifyAuthorization(const Request &req, Privilege_t &level,
                             UserID_t &userid) {
        if (req.has_header("Authorization") == false) {
            throw AuthorizationError("No Authorization header");
        }
        auto token = req.get_header_value("Authorization");
        auto verify = jwt::verify()
                      // We only need an RSA public key to verify tokens
                      .allow_algorithm(jwt::algorithm::rs256(rsa_pub_key, "", "", ""))
                      // We expect token to come from a known authorization server
                      .with_issuer("auth0");
        /* [allow rsa algorithm] */
        auto decoded = jwt::decode(token);
        verify.verify(decoded);
        for (auto &e : decoded.get_payload_json()) {
            if (e.first == "level") level = Privilege_t(std::stoi(e.second.to_str()));
            if (e.first == "username") userid = e.second.to_str();
            if (e.first == "exp") {
                auto exp = std::stoi(e.second.to_str());
                auto exp_time = std::chrono::system_clock::from_time_t(exp);
                auto now = std::chrono::system_clock::now();
                if (now > exp_time) {
                    throw AuthorizationError("Token has expired");
                }
            }
        }
    }

    void GetPostData(const Request &req, const std::string &name,
                     std::string &data) {
        // std::cerr << req.get_file_value(name).content << std::endl;
        picojson::value v;
        std::string err = picojson::parse(v, req.body);
        if (err.empty() == false) throw APIError("JSON Parse error");
        if (!v.contains(name)) throw APIError("No " + name + " field");
        if (v.get(name).is<picojson::array>()) {
            data = "";
            for (auto &e : v.get(name).get<picojson::array>())
                data += e.to_str() + "|";
            if (!data.empty()) data.pop_back();
        } else data = v.get(name).to_str();
    }
    std::string KeywordtoJson(const Keyword_t &keyword) {
        std::string keydata; std::vector<Keyword_t> keys;
        SplitString(keyword, keys, '|');
        for (auto &key : keys)
            keydata += acm::format("\"%s\",", key.c_str());
        if (!keydata.empty()) keydata.pop_back();
        return keydata;
    }


  public:
    MyWebServer() = default;
    ~MyWebServer() = default;
    void Run() {
        std::cerr << "Starting server..." << std::endl;
        Server svr;
        svr.new_task_queue = [] { return new ThreadPool(1); };
        svr.set_default_headers({
            {"Access-Control-Allow-Origin", "*"},
            {"Access-Control-Allow-Methods", "POST, GET, OPTIONS"},
            {"Acesss-Control-Max-Age", "36000"},
            {"Access-Control-Allow-Headers", "*"},
            {"Content-Type", "application/json"}
        });
        // Set server error handler
        svr.set_exception_handler([](const auto & req, auto & res,
        std::exception_ptr ep) {
            auto fmt = "<h1>Error 500</h1><p>%s</p>";
            char buf[BUFSIZ];
            try {
                std::rethrow_exception(ep);
            } catch (Exception &e) {
                res.set_content(acm::format("{\"error\":\"%s\"}", e.what().c_str()),
                                "application/json");
                res.status = StatusCode::Forbidden_403;
                return ;
            } catch (jwt::error::token_verification_exception &e) {
                res.set_content(acm::format("{\"error\":\"%s\"}", e.what()),
                                "application/json");
                res.status = StatusCode::Forbidden_403;
                return;
            } catch (std::exception &e) {
                snprintf(buf, sizeof(buf), fmt, e.what());
                res.set_content(acm::format("{\"error\":\"%s\"}", e.what()),
                                "application/json");
                res.status = StatusCode::Forbidden_403;
            } catch (...) { // See the following NOTE
                snprintf(buf, sizeof(buf), fmt, "Unknown Exception");
            }
            res.set_content(buf, "text/html");
            res.status = StatusCode::InternalServerError_500;
        });
        svr.Options(R"(/.*)", [](const Request & req, Response & res) {
            res.status = StatusCode::OK_200;
        });
        //  /login
        svr.Post("/login", [&](const Request & req, Response & res) {
            // req.files
            std::string userid, password;
            GetPostData(req, "username", userid);
            if (!isValidUserID(userid)) throw APIError("Invalid UserID");
            GetPostData(req, "password", password);
            if (!isValidPassword(password)) throw APIError("Invalid Password");
            User user;
            Users.Login(userid, password, user);
            auto token = jwt::create()
                         .set_issuer("auth0")
                         .set_type("JWT")
                         .set_issued_now()
                         .set_expires_in(std::chrono::seconds{36000})
                         .set_payload_claim("username", jwt::claim(user.userid))
                         .set_payload_claim("level", jwt::claim(std::to_string(int(user.privilege))))
                         .sign(jwt::algorithm::rs256("", rsa_priv_key, "", ""));
            // std::cerr << "Token: " << token << std::endl;
            res.set_content(
                acm::format("{\"data\":{\"token\":\"%s\",\"username\":\"%s\",\"nickname\":\"%s\",\"level\":\"%d\"}}",
                            token.c_str(), user.userid.c_str(), user.username.c_str(), int(user.privilege)),
                "application/json");
            Logs.log(format("%s get login token", user.userid.c_str()));
        });
        svr.Post("/api/searchbook", [&](const Request & req, Response & res) {
            // req.files
            Privilege_t level; UserID_t userid; VerifyAuthorization(req, level, userid);
            if (level < eCustomer) throw AuthorizationError("No privilege");
            std::string searchtype, searchtext;
            GetPostData(req, "searchtype", searchtype);
            GetPostData(req, "searchtext", searchtext);
            std::vector<Book> books;
            std::string data = "{\"data\":[";
            if (searchtype == "All") {
                Books.Show(sAll, "", books);
            } else if (searchtype == "ISBN") {
                if (!isValidISBN(searchtext)) throw APIError("Invalid ISBN");
                Books.Show(sISBN, searchtext, books);
            } else if (searchtype == "Name") {
                if (!isValidBookName(searchtext)) throw APIError("Invalid BookName");
                Books.Show(sName, searchtext, books);
            } else if (searchtype == "Author") {
                if (!isValidAuthor(searchtext)) throw APIError("Invalid Author");
                Books.Show(sAuthor, searchtext, books);
            } else if (searchtype == "Keyword") {
                if (!isValidKeyword(searchtext)) throw APIError("Invalid Keyword");
                Books.Show(sKeyword, searchtext, books);
            } else {
                throw APIError("Unknown searchtype");
            }
            for (auto &book : books) {
                data += acm::format("{\"ISBN\":\"%s\",\"name\":\"%s\",\"author\":\"%s\",\"keywords\":[%s],\"price\":\"%.2lf\"},",
                                    book.ISBN.c_str(), book.name.c_str(), book.author.c_str(),
                                    KeywordtoJson(book.keyword).c_str(),
                                    book.price);
            }
            if (data.size() > 10) data.pop_back();
            data += "]}";
            res.set_content(data, "application/json");
            Logs.log(format("[%s] searchbook searchtype=%s searchtext=%s", userid.c_str(),
                            searchtype.c_str(), searchtext.c_str()));
        });
        svr.Post("/api/purchasebook", [&](const Request & req, Response & res) {
            // req.files
            Privilege_t level; UserID_t userid; VerifyAuthorization(req, level, userid);
            if (level < eCustomer) throw AuthorizationError("No privilege");
            std::string ISBN, quantity;
            GetPostData(req, "ISBN", ISBN);
            if (!isValidISBN(ISBN)) throw APIError("Invalid ISBN");
            GetPostData(req, "quantity", quantity);
            if (!isValidQuantity(quantity)) throw APIError("Invalid Quantity");
            double totalcost;
            Books.Buy(ISBN, std::stoul(quantity), totalcost);
            Logs.trade(totalcost);
            res.set_content(format("{\"data\":{\"totalcost\":\"%.2lf\"}}", totalcost),
                            "application/json");
            Logs.log(format("[%s] purchasebook ISBN=%s quantity=%s", userid.c_str(),
                            ISBN.c_str(), quantity.c_str()));
            Logs.log(format("trade: + %.2lf", totalcost));
        });
        svr.Post("/api/selectbook", [&](const Request & req, Response & res) {
            // req.files
            Privilege_t level; UserID_t userid; VerifyAuthorization(req, level, userid);
            if (level < eAdmin) throw AuthorizationError("No privilege");
            std::string ISBN;
            GetPostData(req, "ISBN", ISBN);
            if (!isValidISBN(ISBN)) throw APIError("Invalid ISBN");
            Book book;
            Books.Select(ISBN, book);
            res.set_content(
                format("{\"data\":{\"ISBN\":\"%s\",\"name\":\"%s\",\"author\":\"%s\",\"keywords\":[%s],\"price\":\"%.2lf\",\"quantity\":\"%lu\"}}",
                       book.ISBN.c_str(), book.name.c_str(), book.author.c_str(),
                       KeywordtoJson(book.keyword).c_str(),
                       book.price, book.quantity),
                "application/json");
            Logs.log(format("[%s] selectbook ISBN=%s", userid.c_str(), ISBN.c_str()));
        });
        svr.Post("/api/modifybook", [&](const Request & req, Response & res) {
            // req.files
            Privilege_t level; UserID_t userid; VerifyAuthorization(req, level, userid);
            if (level < eAdmin) throw AuthorizationError("No privilege");
            std::string oldISBN, ISBN, name, author, keyword, price, quantity;
            GetPostData(req, "oldISBN", oldISBN);
            if (!isValidISBN(oldISBN)) throw APIError("Invalid oldISBN");
            GetPostData(req, "ISBN", ISBN);
            if (!isValidISBN(ISBN)) throw APIError("Invalid ISBN");
            GetPostData(req, "name", name);
            if (!isValidBookName(name)) throw APIError("Invalid BookName");
            GetPostData(req, "author", author);
            if (!isValidAuthor(author)) throw APIError("Invalid Author");
            GetPostData(req, "keywords", keyword);
            if (!isValidKeyword(keyword)) throw APIError("Invalid Keyword");
            GetPostData(req, "price", price);
            if (!isValidPrice(price)) throw APIError("Invalid Price");
            GetPostData(req, "quantity", quantity);
            if (!(quantity == "0"
                  || isValidQuantity(quantity))) throw APIError("Invalid Quantity");
            Book book(ISBN, name, author, keyword, std::stod(price), std::stoul(quantity));
            Books.Modify(oldISBN, book);
            res.set_content("{\"data\": \"success\"}", "application/json");
            Logs.log(format("[%s] modifybook oldISBN=%s ISBN=%s name=%s author=%s keywords=%s price=%s quantity=%s",
                            userid.c_str(), oldISBN.c_str(), ISBN.c_str(), name.c_str(), author.c_str(),
                            keyword.c_str(), price.c_str(), quantity.c_str()));
        });
        svr.Post("/api/importbook", [&](const Request & req, Response & res) {
            // req.files
            Privilege_t level; UserID_t userid; VerifyAuthorization(req, level, userid);
            if (level < eAdmin) throw AuthorizationError("No privilege");
            std::string ISBN, quantity, totalcost;
            // std::cerr << "totalcost:" <<totalcost << std::endl;
            GetPostData(req, "ISBN", ISBN);
            if (!isValidISBN(ISBN)) throw APIError("Invalid ISBN");
            GetPostData(req, "quantity", quantity);
            if (!isValidQuantity(quantity)) throw APIError("Invalid Quantity");
            GetPostData(req, "totalcost", totalcost);
            if (!isValidPrice(totalcost)) throw APIError("Invalid TotalCost");
            Books.Import(ISBN, std::stoul(quantity));
            Logs.trade(-std::stod(totalcost));
            res.set_content("{\"data\": \"success\"}", "application/json");
            Logs.log(format("[%s] importbook ISBN=%s quantity=%s totalcost=%s",
                            userid.c_str(), ISBN.c_str(), quantity.c_str(), totalcost.c_str()));
            Logs.log(format("trade: - %.2lf", std::stod(totalcost)));
        });
        svr.Post("/api/adduser", [&](const Request & req, Response & res) {
            // req.files
            Privilege_t level; UserID_t userid; VerifyAuthorization(req, level, userid);
            if (level < eAdmin) throw AuthorizationError("No privilege");
            std::string _userid, _username, _password, _privilege;
            GetPostData(req, "username", _userid);
            if (!isValidUserID(_userid)) throw APIError("Invalid UserID(username)");
            GetPostData(req, "nickname", _username);
            if (!isValidUsername(_username)) throw APIError("Invalid UserName(nickname)");
            GetPostData(req, "password", _password);
            if (!isValidPassword(_password)) throw APIError("Invalid Password");
            GetPostData(req, "level", _privilege);
            if (!isValidPrivilege(_privilege)) throw APIError("Invalid Privilege(level)");
            if (std::stoi(_privilege) >= level) throw
                APIError("No privilege to create user with higher level");
            Users.UserAdd(_userid, _password, Privilege_t(std::stoi(_privilege)),
                          _username);
            res.set_content("{\"data\": \"success\"}", "application/json");
            Logs.log(format("[%s] adduser username=%s nickname=%s password=%s level=%s",
                            userid.c_str(), _userid.c_str(), _username.c_str(), _password.c_str(),
                            _privilege.c_str()));
        });
        svr.Post("/api/deleteuser", [&](const Request & req, Response & res) {
            // req.files
            Privilege_t level; UserID_t userid; VerifyAuthorization(req, level, userid);
            if (level < eRoot) throw AuthorizationError("No privilege");
            std::string _userid;
            GetPostData(req, "username", _userid);
            if (!isValidUserID(_userid)) throw APIError("Invalid UserID(username)");
            if (UserID_t(_userid) == userid) throw APIError("Cannot delete yourself");
            Users.UserDelete(_userid);
            res.set_content("{\"data\": \"success\"}", "application/json");
            Logs.log(format("[%s] deleteuser username=%s",
                            userid.c_str(), _userid.c_str()));
        });
        svr.Post("/api/changepassword", [&](const Request & req, Response & res) {
            // req.files
            Privilege_t level; UserID_t userid; VerifyAuthorization(req, level, userid);
            if (level < eCustomer) throw AuthorizationError("No privilege");
            std::string _userid, _cur_password, _password;
            GetPostData(req, "username", _userid);
            if (!isValidUserID(_userid)) throw APIError("Invalid UserID(username)");
            GetPostData(req, "cur_password", _cur_password);
            if (!isValidPassword(_cur_password)) throw
                APIError("Invalid Password(cur_password)");
            GetPostData(req, "password", _password);
            if (!isValidPassword(_password)) throw APIError("Invalid Password(password)");
            Users.ChangePassword(_userid, _password, _cur_password, level);
            res.set_content("{\"data\": \"success\"}", "application/json");
            Logs.log(format("[%s] changepassword username=%s cur_password=%s password=%s",
                            userid.c_str(), _userid.c_str(), _cur_password.c_str(), _password.c_str()));
        });
        svr.Post("/api/getlog", [&](const Request & req, Response & res) {
            // req.files
            Privilege_t level; UserID_t userid; VerifyAuthorization(req, level, userid);
            if (level < eRoot) throw AuthorizationError("No privilege");
            std::string logtype, data;
            GetPostData(req, "logtype", logtype);
            if (logtype == "log") {
                Logs.showLog(data);
            } else if (logtype == "trade") {
                Logs.showTrade(data);
            } else if (logtype == "work") {
                Logs.showWork(data);
            } else throw APIError("Unknown logtype");
            res.set_content(data, "text/plain");
        });
        svr.Post("/debug/stop", [&](const Request & req, Response & res) {
            svr.stop();
        });
        Logs.log("Web Server start");
        svr.listen("localhost", 1234);
        Logs.log("Web Server stop");
        std::cerr << "stop Server" << std::endl;
    }
};











}