#include <cstring>
#include <string>
#include <vector>

using namespace std;

template <typename Value_T, const int logsz = 5>
struct Node {
    static const int size = 1 << logsz;
    char *key = nullptr;
    int keyLength = -1;
    Value_T second;
    Node<Value_T, logsz> *children[size];
    inline Node(){
        for(int i = 0; i < size; i++) children[i] = 0;
    }
    inline ~Node() {
        if(~keyLength) delete[] key;
    }
};

inline int findModOf(int n) {
    for(int i = 1; i; i <<= 1) n |= n >> i;
    return n;
}

inline int comp(const char *s1, const char *s2, const int len) {
    int fakelen = len >> 2, lite = fakelen << 2, ite = 0;
    if(!fakelen){
        while(ite ^ len){
            if(s1[ite] ^ s2[ite]) return 0;
            ite++;
        }
        return 1;
    }
    int *fakes1 = (int*) s1, *fakes2 = (int*) s2;
    beg:
    if(fakes1[ite] ^ fakes2[ite]){
        return 0;
    }
    ite++;
    if(ite ^ fakelen)goto beg;
    while(lite ^ len){
        if(s1[lite] ^ s2[lite]) return 0;
        lite++;
    }
    return 1;
}

inline void copy(char *dest, const char *source, const int len) {
    int fakelen = len >> 2, lite = fakelen << 2, ite = 0;
    if(!fakelen){
        while(ite ^ len){
            dest[ite] = source[ite];
            ite++;
        }
        return;
    }
    int *fakedest = (int*) dest, *fakesource = (int*) source;
    beg:
    fakedest[ite] = fakesource[ite];
    ite++;
    if(ite ^ fakelen)goto beg;
    while(lite ^ len){
        dest[lite]=source[lite];
        lite++;
    }
    return;
}

template <typename Value_T, const int logsz = 5, const int LJ = 8>
struct RAT {
    static const int size = 1 << logsz;
    static const int J = 256 << LJ;

    Node<Value_T, logsz> *root;
    vector<Node<Value_T, logsz>*> updates;

    Node<Value_T, logsz> *res, *cur;
    int depth, index, modConst, next, con;

    inline RAT() {
        root = new Node<Value_T, logsz>[J];
    }

    #define calc() key[len - index - 1] + key[index] + depth
    #define rootop() ((key[max(0,len-3)] ^ key[max(0,len-2)] << LJ) & (J - 1))

    inline Node<Value_T, logsz> *end(){
        return nullptr;
    }

    inline Value_T &operator[](string &kkey){
        const char *key = kkey.c_str();
        const int len = kkey.size();
        depth = 0, index = len - 1, modConst = findModOf(len >> 1);
        res = nullptr, cur = root + rootop();
        beg:
        if (cur->keyLength == -1){ 
            res = cur;
        }
        if (cur->keyLength == len && comp(cur->key, key, len)) {
            return cur->second;
        }
        con = calc();
        next = con & (size - 1);
        if (!cur->children[next]) {
            if (!res) {
                cur->children[next] = new Node<Value_T, logsz>;
                updates.push_back(cur->children[next]);
                res = cur->children[next];
            }
            res->key = new char[len];
            copy(res->key, key, len);
            res->keyLength = len;
            return res->second;
        }
        cur = cur->children[next];
        index -= con & modConst;
        if (index >> 31) index += len;
        depth++;
        goto beg;
    }
    
    inline Node<Value_T, logsz> *find(const string &key){
        return find(key.c_str(), key.size());
    }
    inline Node<Value_T, logsz> *insert(const string &key, const Value_T second){
        return insert(key.c_str(), second, key.size());
    }
    inline void erase(const string &key){
        erase(key.c_str(), key.size());
    }

    inline Node<Value_T, logsz> *find(const char *key, const int len) {
        depth = 0, index = len - 1, modConst = findModOf(len >> 1);
        cur = root + rootop();
        beg:
        if (cur->keyLength == len && comp(cur->key, key, len)) { 
            return cur;
        }
        con = calc();
        next = con & (size - 1);
        if (!cur->children[next]){ 
            return nullptr;
        }
        cur = cur->children[next];
        index -= con & modConst;
        if (index >> 31) index += len;
        depth++;
        goto beg;
    }

    inline Node<Value_T, logsz> *insert(const char *key, const Value_T second, const int len) {
        depth = 0, index = len - 1, modConst = findModOf(len >> 1);
        res = nullptr, cur = root + rootop();
        beg:
        if (cur->keyLength == -1) res = cur;
        if (cur->keyLength == len && comp(cur->key, key, len)) {
            cur->second = second;
            return cur;
        }
        con = calc();
        next = con & (size - 1);
        if (!cur->children[next]) {
            if (!res) {
                cur->children[next] = new Node<Value_T, logsz>;
                updates.push_back(cur->children[next]);
                res = cur->children[next];
            }
            res->key = new char[len];
            copy(res->key, key, len);
            res->keyLength = len;
            res->second = second;
            return res;
        }
        cur = cur->children[next];
        index -= con & modConst;
        if (index >> 31) index += len;
        depth++;
        goto beg;
    }
    inline void erase(const char *key, const int len) {
        Node<Value_T, logsz> *to_delete = find(key,len);
        if (to_delete && ~to_delete->keyLength) {
            delete[] to_delete->key;
            to_delete->keyLength = -1;
        }
    }
    ~RAT() {
        delete[] root;
        for (auto update : updates) delete update;
    }
};