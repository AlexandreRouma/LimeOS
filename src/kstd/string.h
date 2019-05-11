#pragma once
#include <stdint.h>
#include <misc/liballoc.h>
#include <vector.h>

class string {
    public:
        string(const string& str);
        string(char* str);
        string();
        ~string();
        string operator+(string str);
        string operator+(char c);
        void operator+=(string str);
        void operator+=(char c);
        char operator[](uint32_t i);
        bool operator==(string str);
        bool operator!=(string str);
        bool operator==(char* str);
        bool operator!=(char* str);
        void operator=(string str);
        void operator=(char* str);
        //operator char*() const { return this->_str; }
        char* toCharArray();
        uint32_t length();
        void reserve(uint32_t len);
        string substring(uint32_t index);
        string substring(uint32_t index, uint32_t length);
        bool startsWith(string str);
        bool endWith(string str);
        string toUpper();
        string toLower();
        char* c_str();
        vector<string> split(char c);
        int lastIndexOf(char c);
        int firstIndexOf(char c);

    private:
        char* _str;
        uint32_t _length;
        bool _init = false;
};

uint32_t strlen(char* str);
bool strcmp(char* str_a, char* str_b);
char* dumpHexByte(uint8_t n);
string itoa(uint32_t n, uint8_t base);
uint32_t strcnt(char* str, char c);
int strfio(char* str, char c);
char* substr(char* str, uint32_t index);
bool strsw(char* str, char* match);
char* strcpy(char* str);