// key2value 数据库

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <fstream>
#include <list>
#include <vector>
#include "fileio.hpp"
#include "utility.hpp"

namespace acm {

template <class key_t, class value_t>
class     blocklist {
  private:
    static const int BLOCK_SIZE = 256;
    static const int MAX_SIZE = 255;
    static const int MIN_SIZE = 64;
    static const int MERGE_SIZE = 200;

    struct pair_t {
        key_t key;
        value_t value;

        pair_t() : key(), value() {}
        pair_t(const key_t &_key, const value_t &_value) : key(_key), value(_value) {}
        pair_t(const pair_t &rhs) : key(rhs.key), value(rhs.value) {}

        ~pair_t() = default;

        pair_t &operator=(const pair_t &rhs) {
            if (this == &rhs) return *this;
            key = rhs.key;
            value = rhs.value;
            return *this;
        }

        bool operator<(const pair_t &rhs) const {
            return Camp(key, rhs.key) == 0 ? value < rhs.value : Camp(key, rhs.key) < 0;
        }

        bool operator==(const pair_t &rhs) const {
            return Camp(key, rhs.key) == 0 && value == rhs.value;
        }

        friend bool operator<(const key_t &lhs, const pair_t &rhs) {
            return Camp(lhs, rhs.key) < 0;
        }

        friend ostream &operator<<(ostream &os, const pair_t &obj) {
            os << obj.key << " : " << obj.value;
            return os;
        }
    };

    struct Nodehead {
        int index; // index of Nodedata in the file
        int count; // count of pairs in this node
        pair_t max_pair; // max pair in this node
    };

    // head 是存放 Nodehead 的链表， free_head 是空闲的 Nodehead 链表。
    std::list<Nodehead> head, free_head; // 嫌麻烦，还是 std::list 算了。
    using list_it = typename std::list<Nodehead>::iterator;

    struct Nodedata {
        pair_t data[BLOCK_SIZE];
        pair_t &operator[](const int &k) { return data[k]; }
    };


    Nodedata cache1, cache2; // 用于读写的缓存

    /* head_file 按顺序存放 Nodehead。其中 info 部分存两个 int: size, free_size。
       分别表示 使用中 Nodehead 数量与 空闲 Nodehead 数量。
       将内容读入后，在内存中以链表存储。
       data_file 存放 Nodedata 按照 head_file.index 索引。
    */
    FileData<Nodehead> head_file;
    FileData<Nodedata> data_file;


    // 创建一个新的 NodeData 并返回 index
    int newnode() {
        int index = 0;
        if (free_head.empty()) {
            index = data_file.writenew(
                        cache1); // 写入一个新的节点 Data, cache1 内容无所谓，只是预留空间。
        } else {
            index = free_head.back().index;
            free_head.pop_back();
        }
        return index;
    }

    // 删除一个节点，并回收
    void recyc(const typename std::list<Nodehead>::iterator &it) {
        free_head.push_back(*it);
        head.erase(it);
    }

    // 找到 pair_t 所对应区块
    list_it findhead(const pair_t &pair) {
        list_it it = head.begin();
        while (it != head.end() && Camp(pair, it->max_pair) > 0) ++it;
        return it;
    }

    list_it findhead(const key_t &key) {
        list_it it = head.begin();
        while (it != head.end() && Camp(key, it->max_pair.key) > 0) ++it;
        return it;
    }

    // 分裂
    bool split(const list_it &it) {
        if (it->count < MAX_SIZE) return false;
        list_it jt = head.insert(std::next(it), Nodehead{newnode(), 0, it->max_pair});
        int k = it->count / 2;
        memcpy(cache2.data, cache1.data + k, sizeof(pair_t) * (it->count - k));
        jt->count = it->count - k;
        it->count = k;
        it->max_pair = cache1[k - 1];
        return true;
    }
    
    // 合并
    void merge(const list_it &it) {
        if (it->count > MIN_SIZE) { }
        else if (it != head.begin() && it->count + std::prev(it)->count <= MERGE_SIZE) {
            list_it jt = std::prev(it);
            data_file.read(cache2, jt->index);
            memcpy(cache1.data + jt->count, cache1.data, sizeof(pair_t) * it->count);
            memcpy(cache1.data, cache2.data, sizeof(pair_t) * jt->count);
            it->count += jt->count;
            recyc(jt);
        } else if (std::next(it) != head.end()
                   && it->count + std::next(it)->count <= MERGE_SIZE) {
            list_it jt = std::next(it);
            data_file.read(cache2, jt->index);
            memcpy(cache1.data + it->count, cache2.data, sizeof(pair_t) * jt->count);
            it->count += jt->count;
            it->max_pair = jt->max_pair;
            recyc(jt);
        }
    }

  public:
    blocklist(const char *head_file_name, const char *data_file_name)
        : head_file(head_file_name), data_file(data_file_name) {
        if (!head_file.exist())  {
            // 创建新文件，写入一个空节点
            head_file.init();
            data_file.init();
            head_file.write_info(1, 1);
            head_file.write_info(0, 2);
            cache1[0] = pair_t();
            head.push_back(Nodehead{data_file.writenew(cache1), 1, cache1[0]});
        } else {
            // 从已有文件中读取 head, free_head
            head_file.open();
            data_file.open();
            int size, free_size;
            head_file.get_info(size, 1);
            head_file.get_info(free_size, 2);
            head.resize(size);
            free_head.resize(free_size);
            list_it it = head.begin();
            head_file.setread(0);
            for (int i = 0; i < size; ++i) {
                head_file.read(*it);
                ++it;
            }
            it = free_head.begin();
            for (int i = 0; i < free_size; ++i) {
                head_file.read(*it);
                ++it;
            }
        }
    }

    ~blocklist() {
        int size = head.size(), free_size = free_head.size();
        head_file.write_info(size, 1);
        head_file.write_info(free_size, 2);
        list_it it = head.begin();
        head_file.setwrite();
        for (int i = 0; i < size; ++i) {
            head_file.write(*it);
            ++it;
        }
        it = free_head.begin();
        for (int i = 0; i < free_size; ++i) {
            head_file.write(*it);
        }
    }

    bool empty() const {
        return head.size() == 1 && head.begin()->count == 1;
    }

  public:
    // 插入键值对 [key, value]
    void insert(const key_t &key, const value_t &value) {
        pair_t pair(key, value);
        list_it it = findhead(pair);
        if (it == head.end()) {
            --it;
            data_file.read(cache1, it->index);
            cache1[it->count++] = pair;
            it->max_pair = pair;
        } else {
            data_file.read(cache1, it->index);
            int k = 0;
            while (k < it->count && Camp(pair, cache1[k]) > 0) ++k;
            for (int i = it->count; i > k; --i) cache1[i] = cache1[i - 1];
            cache1[k] = pair;
            ++it->count;
        }
        if (split(it)) {
            data_file.update(cache1, it->index);
            data_file.update(cache2, std::next(it)->index);
        } else {
            data_file.update(cache1, it->index);
        }
    }

    // 插入键值对 [key, value] 保证 key 对应 value 唯一，若已有 key 则不创建
    bool insertkey(const key_t &key, const value_t &value) {
        pair_t pair(key, value);
        list_it it = findhead(key);
        if (it == head.end()) {
            --it;
            data_file.read(cache1, it->index);
            cache1[it->count++] = pair;
            it->max_pair = pair;
        } else {
            data_file.read(cache1, it->index);
            int k = 0;
            while (k < it->count && Camp(key, cache1[k].key) > 0) ++k;
            if (Camp(key, cache1[k].key) == 0) return false; // 已有 key
            for (int i = it->count; i > k; --i) cache1[i] = cache1[i - 1];
            cache1[k] = pair;
            ++it->count;
        }
        if (split(it)) {
            data_file.update(cache1, it->index);
            data_file.update(cache2, std::next(it)->index);
        } else {
            data_file.update(cache1, it->index);
        }
        return true;
    }

    // 删除键值对 [key, value] 成功返回 true 失败返回 false 
    bool remove(const key_t &key, const value_t &value) {
        pair_t pair(key, value);
        list_it it = findhead(pair);
        if (it == head.end()) return false; // 找不到键值对
        data_file.read(cache1, it->index);
        int k = 0;
        while (k < it->count && Camp(pair, cache1[k]) > 0) ++k;
        if (k == it->count || Camp(pair, cache1[k]) != 0) return false; // 找不到键值对
        for (int i = k; i < it->count - 1; ++i) cache1[i] = cache1[i + 1];
        --it->count;
        merge(it);
        data_file.update(cache1, it->index);
        return true;
    }

    // 根据 key, 删除键值对 [key, value] ，应该认为 key 对应 value 唯一,成功返回 true 失败返回 false 
    bool removekey(const key_t &key) {
        list_it it = findhead(key);
        if (it == head.end()) return false; // 找不到键值对
        data_file.read(cache1, it->index);
        int k = 0;
        while (k < it->count && Camp(key, cache1[k].key) > 0) ++k;
        if (k == it->count || Camp(key, cache1[k].key) != 0) return false; // 找不到键值对
        for (int i = k; i < it->count - 1; ++i) cache1[i] = cache1[i + 1];
        --it->count;
        merge(it);
        data_file.update(cache1, it->index);
        return true;
    }

    // 根据 key, 修改对应 value ，应该认为 key 对应 value 唯一, 成功返回 true 失败返回 false
    bool modify(const key_t &key, const value_t &value) {
        list_it it = findhead(key);
        if (it == head.end()) return false; // 找不到键值对
        data_file.read(cache1, it->index);
        int k = 0;
        while (k < it->count && Camp(key, cache1[k].key) > 0) ++k;
        if (k == it->count || Camp(key, cache1[k].key) != 0) return false; // 找不到键值对
        cache1[k].value = value;
        return true;
    }

    // 获取键 [key] 对应所有值, 添加到 res 末尾, 若 key 为空，则获取所有值
    void fetchall(const key_t &key, std::vector<value_t> &res) {
        if (key.empty()) {
            bool flag = 0;
            for (auto &it : head) {
                data_file.read(cache1, it.index);
                for (int i = 0; i < it.count; ++i) {
                    if (flag )res.push_back(cache1[i].value);
                    flag = 1;
                }
            }
        }
        list_it it = findhead(key);
        while (it != head.end()) {
            data_file.read(cache1, it->index);
            int k = 0;
            while (k < it->count && cache1[k].key < key) ++k;
            while (k < it->count && key == cache1[k].key) {
                res.push_back(cache1[k].value);
                ++k;
            }
            if (k == it->count && it->max_pair.key == key) {
                ++it;
            } else break;
        }
    }

    // 显示所有数据，调试用
    void ShowAll() {
        bool flag = 0;
        for (auto &it : head) {
            data_file.read(cache1, it.index);
            for (int i = 0; i < it.count; ++i) {
                if (flag) std::cerr << cache1[i] << std::endl;
                flag = 1;
            }
        }
    }

};

// future: 实现 B+ 树，

template <class key_t, class value_t>
using database = blocklist<key_t, value_t>;

}


#endif // DATABASE_HPP