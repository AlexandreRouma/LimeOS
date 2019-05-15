#include <string.h>
#include <stdint.h>
#include <kapi.h>

uint32_t strlen(char* str){
    uint32_t len = 0;
	while (str[len])
		len++;
	return len;
}

bool strcmp(char* str_a, char* str_b) {
	uint32_t len_a = strlen(str_a);
	uint32_t len_b = strlen(str_b);
	if (len_a != len_b) {
		return false;
	}
	for (uint32_t i = 0; i < len_a; i++) {
		if (str_a[i] != str_b[i]) {
			return false;
		}
	}
	return true;
}

char HEX_ALPHABET[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

char* dumpHexByte(uint8_t n) {
    char* rtn = "00";
    rtn[1] = HEX_ALPHABET[n & 0x0F];
    rtn[0] = HEX_ALPHABET[(n & 0xF0) >> 4];
    return rtn;
}

string itoa(uint32_t _n, uint8_t base) {
	uint32_t n = _n;
	if (n == 0) {
		return "0";
	}
	string ret;
	while (n > 0) {
		uint32_t digit = n % base;
		string str = "";
		str += HEX_ALPHABET[digit];
		str += ret;
		ret = str;
		n -= digit;
		n /= base;
	}
	return ret;
}

uint32_t strcnt(char* str, char c) {
	uint32_t count = 0;
	uint32_t len = strlen(str);
	for (uint32_t i = 0; i < len; i++) {
		if (str[i] == c) {
			count++;
		}
	}
	return count;
}

int strfio(char* str, char c) {
	uint32_t index = 0;
	uint32_t len = strlen(str);
	for (uint32_t i = 0; i < len; i++) {
		if (str[i] == c) {
			return i;
		}
	}
	return -1;
}

char* substr(char* str, uint32_t index) {
	return str + index;
}

bool strsw(char* str, char* match) {
	uint32_t len = strlen(match);
	for (uint32_t i = 0; i < len; i++) {
		if (str[i] != match[i]) {
			return false;
		}
	}
	return true;
}

// <=============================== STRING CLASS ===============================>

string::string(const string& str) {
	this->_length = strlen(str._str);
	this->_str = (char*)kapi::api.mm.malloc(this->_length + 1);
	kapi::api.mm.memcpy(this->_str, str._str, this->_length);
	this->_str[this->_length] = 0x00;
	_init = true;
}

string::string(char* str) {
	this->_length = strlen(str);
	this->_str = (char*)kapi::api.mm.malloc(this->_length + 1);
	kapi::api.mm.memcpy(this->_str, str, this->_length);
	this->_str[this->_length] = 0x00;
	_init = true;
}

string::string() {
	this->_length = 0;
	this->_str = (char*)kapi::api.mm.malloc(1);
	this->_str[0] = 0x00;
	_init = true;
}

string::~string() {
	this->_length = 0;
	kapi::api.mm.free(this->_str);
}

string string::operator+(string str) {
	string ret;
	ret = this->_str;
	ret += str;
	return ret;
}

string string::operator+(char c) {
	string ret;
	ret = this->toCharArray();
	ret += c;
	return ret;
}

void string::operator+=(string str) {
	this->_str = (char*)kapi::api.mm.realloc(this->_str, this->_length + str.length() + 1);
	kapi::api.mm.memcpy(this->_str + this->_length, str.toCharArray(), str.length());
	this->_str[this->_length + str.length()] = 0x00;
	this->_length += str.length();
}

void string::operator+=(char c) {
	this->_str = (char*)kapi::api.mm.realloc(this->_str, this->_length + 2);
	this->_str[this->_length] = c;
	this->_str[this->_length + 1] = 0x00;
	this->_length++;
}

char string::operator[](uint32_t i) {
	if (i < this->_length) {
		return this->_str[i];
	}
	// TODO: Add kernel panic
	//kernel_panic(0xC0FEFE, "Array out of bounds");
	return 0x00;
}

bool string::operator==(string str) {
	return strcmp(this->_str, str.toCharArray());
}

bool string::operator!=(string str) {
	return !strcmp(this->_str, str.toCharArray());
}

bool string::operator==(char* str) {
	return strcmp(this->_str, str);
}

bool string::operator!=(char* str) {
	return !strcmp(this->_str, str);
}

void string::operator=(string str) {
	if (_init == true) {
		kapi::api.mm.free(this->_str);
	}
	this->_length = str.length();
	this->_str = (char*)kapi::api.mm.malloc(this->_length + 1);
	kapi::api.mm.memcpy(this->_str, str.toCharArray(), this->_length);
	this->_str[this->_length] = 0x00;
}

void string::operator=(char* str) {
	if (_init == true) {
		kapi::api.mm.free(this->_str);
	}
	this->_length = strlen(str);
	this->_str = (char*)kapi::api.mm.malloc(this->_length + 1);
	kapi::api.mm.memcpy(this->_str, str, this->_length);
	this->_str[this->_length] = 0x00;
}

uint32_t string::length() {
	return this->_length;
}

char* string::toCharArray() {
	return this->_str;
}

void string::reserve(uint32_t len) {
	if (len > this->_length) {
		this->_str = (char*)kapi::api.mm.realloc(this->_str, len + 1);
	}
}

string string::substring(uint32_t index) {
	string ret;
	if (index < this->_length) {
		ret = (this->toCharArray() + index);
	}
	return ret;
}

string string::substring(uint32_t index, uint32_t length) {
	string ret;
	if (index + length <= this->_length) {
		for (uint32_t i = index; i < index + length; i++) {
			ret += this->_str[i];
		}
	}
	return ret;
}

bool string::startsWith(string str) {
	if (str.length() > this->_length) {
		return false;
	}
	uint32_t len = str.length();
	for (uint32_t i = 0; i < len; i++) {
		if (this->_str[i] != str[i]) {
			return false;
		}
	}
	return true;
}

bool string::endWith(string str) {
	if (str.length() > this->_length) {
		return false;
	}
	uint32_t len = str.length();
	for (uint32_t i = 0; i < len; i++) {
		if (this->_str[this->_length - len + i] != str[i]) {
			return false;
		}
	}
	return true;
}

string string::toUpper() {
	string ret;
	for (uint32_t i = 0; i < this->_length; i++) {
		char c = this->_str[i];
		if (c >= 97 && c <= 122) {
			c &= 0b11011111;
		}
		ret += c;
	}
	return ret;
}

string string::toLower() {
	string ret;
	for (uint32_t i = 0; i < this->_length; i++) {
		char c = this->_str[i];
		if (c >= 65 && c <= 90) {
			c |= 0b00100000;
		}
		ret += c;
	}
	return ret;
}

char* string::c_str() {
	return this->_str;
}

vector<string> string::split(char c) {
	vector<string> v;
	int last = 0;
	for (int i = 0; i < _length; i++) {
		if (_str[i] == c) {
			string str = substring(last, i - last);
			v.push_back(str);
			last = i + 1;
		}
	}
	string str = substring(last, _length - last);
	v.push_back(str);
	return v;
}

int string::lastIndexOf(char c) {
	for (int i = _length - 1; i >= 0; i--) {
		if (_str[i] == c) {
			return i;
		}
	}
	return -1;
}

int string::firstIndexOf(char c) {
	for (int i = 0; i < _length; i++) {
		if (_str[i] == c) {
			return i;
		}
	}
	return -1;
}

// <=============================== STRING CLASS ===============================>