# Book Store System Web

这是 Book Store System 的 Web 部分。前后端分离，前端使用 [Vue.js](https://cn.vuejs.org/)，后端使用 [cpp-httplib](https://github.com/yhirose/cpp-httplib) 搭建。

## Learn something

- http://www.codebaoku.com/jwt/jwt-index.html
- https://learnku.com/laravel/t/6433/vuejs-implements-front-end-separation-login-verification-and-page-auto-jump



## frontend

```sh
cd frontend
```

### Project Setup

```sh
npm install
```

### Compile and Hot-Reload for Development

```sh
npm run dev
```

### Compile and Minify for Production

```sh
npm run build
```

## backend

使用了开源库 [jwt-cpp](https://github.com/Thalhammer/jwt-cpp) （[How to install](https://github.com/Thalhammer/jwt-cpp/blob/master/docs/install.md#Cmake)）， [cpp-httplib](https://github.com/yhirose/cpp-httplib)（这个直接放在 `/src/include/httplib.h`）

### Setup


```sh
mkdir backend && cd backend
cmake ../.. -DWEB_MODE=ON
make
```

### Run

```sh
./code -s
```

Runing on `localhost:1234`

额，因为技术原因，要停止服务只能 `POST /debug/stop`

### API

前后端交互的 API，登录认证使用 JWT，所有 API 使用 POST 方法，若无特殊说明，返回的都是 JSON 格式的数据。各种命令的要求与命令行的要求类似，若无特殊声明则与 [requirements.md](../requirements.md) 中的要求相同。

```http
Content-Type: application/json;charset=utf-8
```

成功获取数据后会返回 `200` 状态码，
```json
{
    "data": ...
}
```
如果失败则返回 `403` 状态码，
```json
{
    "error": "Error Message"
}
```

#### 登录
- URL: `/login`
- Method: `POST`
- request:
```json
{
    "username": "UserID",
    "password": "PassWord"
}
```

- response:
```json
{
    "data": {
        "token": "TOKEN",
        "username": "UserID",
        "nickname": "Username",
        "level": "Privilege"
    }
}
```

偷懒了，其实 TOKEN 中是带有 `username`、`nickname`、`level` 等信息的，~~但是懒得写前端的 TOKEN 解析，所有就直接传了过来~~。不过后端会解析并验证传回来的 token 中的 `username`, `level` 是否被篡改。

接下来的 API 都需要使用 JWT 进行身份验证，你需要在设置 `headers.Authorization = token`，其中 `token` 是登录成功后返回的 `token` 值。

#### 查询图书
- LEVEL >= 1
- URL: `/api/searchbook`
- Method: `POST`
- request:
```json
{
    "searchtype": "",
    "searchtext": ""
}
```
其中 `searchtype` 为查询类型，`searchtext` 为查询内容，查询类型有 `Name`、`Author`、`ISBN`、`Keyword`、`All`。

如果 `searchtype` 为 `All`，则无视 `searchtext` 的值，返回所有图书。 若 `searchtype` 不为 `All`，则 `searchtext` 为查询内容，返回查询结果，此时 `searchtext` 不能为空。

- response:
```json
{
    "data": [
        {
            "ISBN": "ISBN",
            "name": "BookName",
            "author": "Author",
            "keywords": [
                "keyword1",
                "keyword2",
                ...
            ],
            "price": "10.00"
        },
        ...
    ]
}
```

#### 购买图书
- LEVEL >= 1
- URL: `/api/purchasebook`
- Method: `POST`
- request:
```json
{
    "ISBN": "ISBN",
    "quantity": 1
}
```
其中 `ISBN` 为图书的 ISBN 编号，`quantity` 为购买数量。

- response:
```json
{
    "data": {
        "totalcost": "10.00"
    }
}
```
其中 `totalcost` 为购买图书的总价。

#### 选择图书
- LEVEL >= 3
- URL: `/api/selectbook`
- Method: `POST`
- request:
```json
{
    "ISBN": "ISBN"
}
```
其中 `ISBN` 为图书的 ISBN 编号。若无该图书，则会创建一本新的图书。

- response:
```json
{
    "data": {
        "ISBN": "ISBN",
        "name": "BookName",
        "author": "Author",
        "keywords": [
            "keyword1",
            "keyword2",
            ...
        ],
        "price": "10.00",
        "quantity": "100"
    }
}
```

#### 修改图书
- LEVEL >= 3
- URL: `/api/modifybook`
- Method: `POST`
- request:
```json
{
    "oldISBN": "ISBN",
    "ISBN": "ISBN",
    "name": "BookName",
    "author": "Author",
    "keywords": [
        "keyword1",
        "keyword2",
        ...
    ],
    "price": "10.00"
}
```
其中 `ISBN` 为图书的 ISBN 编号，`name` 为图书的名称，`author` 为图书的作者，`keywords` 为图书的关键词，`price` 为图书的价格。需要保证 `oldISBN` 是存在的，修改后的 `ISBN` 不能与已有书籍重合。

- response:
```json
{
    "data": "success"
}
```

#### 图书进货
- LEVEL >= 3
- URL: `/api/importbook`
- Method: `POST`
- request:
```json
{
    "ISBN": "ISBN",
    "quantity": "1",
    "totalcost": "10.00"
}
```
其中 `ISBN` 为图书的 ISBN 编号，`quantity` 为进货数量，`totalcost` 为进货价格。
- response:
```json
{
    "data": "success"
}
```

#### 添加用户
- LEVEL >= 3
- URL: `/api/adduser`
- Method: `POST`
- request:
```json
{
    "username": "UserID",
    "nickname": "Username",
    "password": "PassWord",
    "level": "Privilege"
}
```

这里不过多解释，每项参数都是必须的。要求同 [requirements.md](../requirements.md)


- response:
```json
{
    "data": "success"
}
```

#### 删除用户
- LEVEL >= 7
- URL: `/api/deleteuser`
- Method: `POST`
- request:
```json
{
    "username": "UserID"
}
```

在登录状态下的用户也可以被删除，不能删除比自己权限高的用户。


- response:
```json
{
    "data": "success"
}
```

#### 修改密码
- LEVEL >= 1
- URL: `/api/changepassword`
- Method: `POST`
- request:
```json
{
    "username": "UserID",
    "cur_password": "CurrentPassWord",
    "password": "PassWord",
}
```

这里不过多解释，每项参数都是必须的，如果修改对象的 level 比自己低，直接无视 `cur_password` 直接修改密码。

- response:
```json
{
    "data": "success"
}
```

#### 获取日志

- LEVEL >= 7
- URL: `/api/getlog`
- Method: `POST`
- request:
```json
{
    "logtype": "logtype",
}
```
`logtype` 可以为 `log` 系统日志 / `work` 员工日志 / `trade` 交易日志。
- response:
```plain
logs...
....
```
注意返回值是纯文本 `Content-Type: text/plain;charset=utf-8`，日志每一条以 `\n` 分隔。

