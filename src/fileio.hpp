// 一个文件读写类

#ifndef FILEIO_HPP
#define FILEIO_HPP

#include <fstream>
#include <string>

using std::fstream;
using std::string;

namespace acm {

template<class T, int info_len = 2>
class File {
  private:
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);
  public:
    File() = default;

    File(const string &file_name) : file_name(file_name) {}

    ~File() {
        file.close();
    }

    // init the file: create file if not exist or clear the file
    void init(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::out | std::ios::binary);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    }

    // check if the file exist
    bool exist() {
        file.open(file_name, std::ios::in);
        bool res = file.is_open();
        file.close();
        return res;
    }

    // just open the file
    void open(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    }

    //读出第 n 个 int 的值赋给 tmp，1_base
    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        file.seekg(sizeof(int) * (n - 1));
        file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
    }

    //将 tmp 写入第 n 个 int 的位置，1_base
    void write_info(int tmp, int n) {
        if (n > info_len) return;
        file.seekp(sizeof(int) * (n - 1));
        file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    }

    // 在末尾写入
    int writenew(T &t) {
        int index;
        file.seekp(0, std::ios::end);
        index = file.tellp();
        index = (index - sizeof(int) * info_len) / sizeofT;
        file.write(reinterpret_cast<const char *>(&t), sizeofT);
        return index;
    }

    //用 t 的值更新位置索引 index 对应的对象，保证调用的 index 都是由 write 函数产生
    void update(T &t, const int index) {
        file.seekp(index * sizeofT + sizeof(int) * info_len);
        file.write(reinterpret_cast<const char *>(&t), sizeofT);
    }

    //读出位置索引 index 对应的 T 对象的值并赋值给 t，保证调用的 index 都是由 write 函数产生
    void read(T &t, const int index) {
        file.seekg(index * sizeofT + sizeof(int) * info_len);
        file.read(reinterpret_cast<char *>(&t), sizeofT);
    }

    // 设置读指针
    void setread(const int index = 0) {
        file.seekg(index * sizeofT + sizeof(int) * info_len);
    }

    // 从当前位置读
    void read(T &t) {
        file.read(reinterpret_cast<char *>(&t), sizeofT);
    }

    // 设置写指针
    void setwrite(const int index = 0) {
        file.seekp(index * sizeofT + sizeof(int) * info_len);
    }

    // 从当前位置写入
    void write(T &t) {
        file.write(reinterpret_cast<const char *>(&t), sizeofT);
    }

};

}

#endif // FILEIO_HPP