#pragma once
#include <stdint.h>
#include <vector.h>
#include <temp_vga/terminal.h>

template <class K, class T>
class map {
public:
    map();
    ~map();
    void insert(K key, T item);
    T & operator[](K key);
    bool exists(K key);
    vector<K>& getKeys();
    vector<T>& getItems();
    void reserve(uint32_t count);

private:
    vector<K> _keys;
    vector<T> _items;
    bool _init = false;
};

template <class K, class T>
map<K,T>::map() {
    _keys = vector<K>();
    _items = vector<T>();
}

template <class K, class T>
map<K,T>::~map() {
    //delete &_keys;
}

template <class K, class T>
void map<K,T>::insert(K key, T item) {
    _keys.push_back(key);
    _items.push_back(item);
}

template <class K, class T>
T & map<K,T>::operator[](K key) {
    for (int i = 0; i < _keys.size(); i++) {
        if (_keys[i] == key) {
            return _items[i];
        }
    }
    // TODO: fix this
    return _items[0];
}

template <class K, class T>
bool map<K,T>::exists(K key) {
    for (int i = 0; i < _keys.size(); i++) {
        if (_keys[i] == key) {
            return true;
        }
    }
    return false;
}

template <class K, class T>
vector<K>& map<K,T>::getKeys() {
    return _keys;
}

template <class K, class T>
vector<T>& map<K,T>::getItems() {
    return _items;
}


template <class K, class T>
void map<K,T>::reserve(uint32_t count) {
    _keys.reserve(count);
    _items.reserve(count);
}