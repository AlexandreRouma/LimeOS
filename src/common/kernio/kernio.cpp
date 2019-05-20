#include <kernio/kernio.h>
#include <temp_vga/terminal.h>
#include <stdarg.h>

namespace kio {
    stream_t stdout;

    void print(char* str) {
        stream::write(stdout, str, strlen(str));
    }

    void print(string str) {
        stream::write(stdout, str.c_str(), str.length());
    }
    
    void println(char* str) {
        char* _str = (char*)malloc(strlen(str) + 2);
        memcpy(_str, str, strlen(str));
        _str[strlen(str)] = '\n';
        _str[strlen(str) + 1] = 0;
        stream::write(stdout, _str, strlen(_str));
        free(_str);
    }
    
    void println(string str) {
        string _str = str;
        _str += '\n';
        stream::write(stdout, _str.c_str(), _str.length());
    }

    bool busy = false;

    void printf(string str, ...) {
        string _str = str;
        string out = "";
        va_list arguments;
        va_start(arguments, str);
        for (int i = 0; i < _str.length(); i++) {
            if (_str[i] == '%') {
                i++;
                int padding = 0;
                char padChar;
                
                if (_str[i] < 'A' || _str[i] > 'z') {
                    padChar = _str[i];
                    i++;
                    bool first = true;
                    for (int j = i; j < _str.length(); j++) {
                        if (_str[i] >= 0x30 && _str[i] <= 0x39) {
                            if (!first) {
                                padding *= 10;
                            }
                            padding += _str[i] - 0x30;
                            first = false;
                            i++;
                        }
                        else {
                            break;
                        }
                    }
                }
                char length = 'i'; // int
                if (_str[i] == 'h') {
                    length = 'h';
                    i++;
                }
                if (_str[i] == 'l') {
                    length = 'l';
                    i++;
                }
                
                // Actual printing;
                // TODO: Have sign ed printing, and float printing (Needs custom itoa)
                string tmp = "";
                if (_str[i] == 'u') {
                    if (length == 'i') {
                        tmp = itoa(va_arg(arguments, uint32_t), 10);
                    }
                    if (length == 'h') {
                        tmp = itoa(va_arg(arguments, uint16_t), 10);
                    }
                    if (length == 'l') {
                        tmp = itoa(va_arg(arguments, uint64_t), 10);
                    }
                }
                if (_str[i] == 'o') {
                    if (length == 'i') {
                        tmp = itoa(va_arg(arguments, uint32_t), 8);
                    }
                    if (length == 'h') {
                        tmp = itoa(va_arg(arguments, uint16_t), 8);
                    }
                    if (length == 'l') {
                        tmp = itoa(va_arg(arguments, uint64_t), 8);
                    }
                }
                if (_str[i] == 'x') {
                    if (length == 'i') {
                        tmp = itoa(va_arg(arguments, uint32_t), 16);
                    }
                    if (length == 'h') {
                        tmp = itoa(va_arg(arguments, uint16_t), 16);
                    }
                    if (length == 'l') {
                        tmp = itoa(va_arg(arguments, uint64_t), 16);
                    }
                    tmp = tmp.toLower();
                }
                if (_str[i] == 'X') {
                    if (length == 'i') {
                        tmp = itoa(va_arg(arguments, uint32_t), 16);
                    }
                    if (length == 'h') {
                        tmp = itoa(va_arg(arguments, uint16_t), 16);
                    }
                    if (length == 'l') {
                        tmp = itoa(va_arg(arguments, uint64_t), 16);
                    }
                }
                // Pad here
                char pad[2] = {padChar, 0};
                if (padding > tmp.length()) {
                    int len = tmp.length();
                    for (int j = 0; j < padding - len; j++) {
                        tmp = string(&pad[0]) + tmp;
                    }
                }

                out += tmp;
            }
            else {
                out += _str[i];
            }
        }
        va_end(arguments);
        print(out);
    }

    void setFore(uint8_t color) {
        if (color > 7) {
            printf("\x1B[1;%um", (color - 8) + 30);
        }
        else {
            printf("\x1B[%um", color + 30);
        }
    }

    void setBack(uint8_t color) {
        if (color > 7) {
            printf("\x1B[1;%um", (color - 8) + 40);
        }
        else {
            printf("\x1B[%um", color + 40);
        }
    }
    
    void setCursor(uint32_t x, uint32_t y) {
        printf("\x1B[%u;%uH", y, x);
    }
    
    void clear() {
        // TODO: Implement
    }
}