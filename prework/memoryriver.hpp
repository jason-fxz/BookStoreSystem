#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>
#include <iostream>
#include <vector>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T, int info_len = 2>
class MemoryRiver {
private:
    // std::vector<int> free;
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);
public:
    MemoryRiver() = default;

    MemoryRiver(const string& file_name) : file_name(file_name) {}

    ~MemoryRiver() {
        file.close();
    }

    // init the file: create file if not exist or clear the file
    void initialise(string FN = "") {
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

    //在文件合适位置写入类对象 t，并返回写入的位置索引 index
    //位置索引意味着当输入正确的位置索引 index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引 index 可以取为对象写入的起始位置
    int write(T &t) {
        int index;
        // if (free.empty()) {
        file.seekp(0, std::ios::end);
        index = file.tellp();
        // } else {
            // index = free.back();
            // free.pop_back();
            // file.seekp(index);
        // }
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

    //删除位置索引 index 对应的对象 (不涉及空间回收时，可忽略此函数)，保证调用的 index 都是由 write 函数产生
    void Delete(int index) {
        // free.push_back(index);
    }
};


#endif //BPT_MEMORYRIVER_HPP