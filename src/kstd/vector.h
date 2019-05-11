#pragma once
#include <stdint.h>
#include <misc/liballoc.h>
#include <misc/memory.h>

template <class T>
class vector {
public:
    vector();
    vector(const vector<T>& v);
    void push_back(T item);
    T pop_back();
    void push_front(T item);
    T pop_front();
    uint32_t size();
    void remove(uint32_t index);

     T & operator[](uint32_t index);
     vector<T> & operator=(const vector<T>& v);

    ~vector();

private:
    T* _items;
    uint32_t itemCount = 0;
    bool _init = false;
};

template<class T>
vector<T>::vector() {
    _items = (T*)malloc(sizeof(T));
    itemCount = 0;
    _init = true;
}

template<class T>
vector<T>::vector(const vector<T>& v) {
    _items = (T*)malloc(sizeof(T) * v.itemCount);
    memcpy(_items, v._items, sizeof(T) * v.itemCount);
    itemCount = v.itemCount;
    _init = true;
}

template<class T>
vector<T>::~vector() {
    free(_items);
    _items = (T*)0;
    itemCount = 0;
    _init = true;
}

template<class T>
void vector<T>::push_back(T item) {
    itemCount++;
    _items = (T*)realloc(_items, itemCount * sizeof(T));
    _items[itemCount - 1] = item;
}

template<class T>
T vector<T>::pop_back() {
    if (itemCount == 0) {
        return NULL;
    }
    itemCount--;
    T item = _items[itemCount];
    _items = (T*)realloc(_items, itemCount * sizeof(T));
    return item;
}

template<class T>
void vector<T>::push_front(T item) {
    itemCount++;
    _items = (T*)realloc(_items, itemCount * sizeof(T));
    memcpy(_items + sizeof(T), _items, (itemCount - 1) * sizeof(T));
    _items[0] = item;
}

template<class T>
T vector<T>::pop_front() {
    if (itemCount == 0) {
        return NULL;
    }
    itemCount--;
    T item = _items[0];
    memcpy(&_items[0], &_items[1], itemCount * sizeof(T));
    _items = (T*)realloc(_items, itemCount * sizeof(T));
    return item;
}

template<class T>
void vector<T>::remove(uint32_t index) {
    memcpy(&_items[index], &_items[index + 1], (itemCount - (index + 1)) * sizeof(T));
    itemCount--;
    _items = (T*)realloc(_items, itemCount * sizeof(T));
}

template<class T>
uint32_t vector<T>::size() {
    return itemCount;
}

template<class T>
T& vector<T>::operator[](uint32_t index) {
    return _items[index];
}

template<class T>
vector<T>& vector<T>::operator=(const vector<T>& v) {
    if (_init) {
        free(_items);
    }
    _items = (T*)malloc(sizeof(T) * v.itemCount);
    memcpy(_items, v._items, sizeof(T) * v.itemCount);
    itemCount = v.itemCount;
    return *this;
}