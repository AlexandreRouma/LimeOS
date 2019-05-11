#include <temp_vga/terminal.h>
#include <string.h>
#include <misc/cpuio.h>
#include <misc/memory.h>

uint8_t text_color = 0x0F;
uint16_t* frame_buffer = (uint16_t*)0xB8000;
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;
uint8_t terminal_width = 0;
uint8_t terminal_height = 0;
uint8_t last_clear_color = 0x00;

Terminal_Class Terminal;

void Terminal_Class::init(uint8_t w, uint8_t h) {
    terminal_width = w;
    terminal_height = h;
}

void Terminal_Class::setColor(char color) {
    text_color = color;
}

void Terminal_Class::putcar(char c, uint8_t x, uint8_t y) {
    frame_buffer[x + (terminal_width * y)] = c | (text_color << 8);
}

void Terminal_Class::print(char* str) {
    for (uint32_t i = 0; i < strlen(str); i++) {
        if (str[i] == '\n'){
            newLine();
        }
        else if (cursor_x == terminal_width) {
            newLine();
        }
        else {
            frame_buffer[cursor_x + (terminal_width * cursor_y)] = str[i] | (text_color << 8);
            cursor_x++;
        }
    }
    setCursor(cursor_x, cursor_y);
}

void Terminal_Class::println(char* str) {
    for (uint32_t i = 0; i < strlen(str); i++) {
        if (str[i] == '\n'){
            newLine();
        }
        else if (cursor_x == terminal_width) {
            newLine();
        }
        else {
            frame_buffer[cursor_x + (terminal_width * cursor_y)] = str[i] | (text_color << 8);
            cursor_x++;
        }
    }
    newLine();
    setCursor(cursor_x, cursor_y);
}

void Terminal_Class::print(string str) {
    for (uint32_t i = 0; i < str.length(); i++) {
        if (str[i] == '\n'){
            newLine();
        }
        else if (cursor_x == terminal_width) {
            newLine();
        }
        else {
            frame_buffer[cursor_x + (terminal_width * cursor_y)] = str[i] | (text_color << 8);
            cursor_x++;
        }
    }
    setCursor(cursor_x, cursor_y);
}

void Terminal_Class::println(string str) {
    for (uint32_t i = 0; i < str.length(); i++) {
        if (str[i] == '\n'){
            newLine();
        }
        else if (cursor_x == terminal_width) {
            newLine();
        }
        else {
            frame_buffer[cursor_x + (terminal_width * cursor_y)] = str[i] | (text_color << 8);
            cursor_x++;
        }
    }
    newLine();
    setCursor(cursor_x, cursor_y);
}

void Terminal_Class::setCursor(uint8_t x, uint8_t y)  {
    frame_buffer[x + (terminal_width * y)] = (frame_buffer[x + (terminal_width * y)] & 0x00FF) | (text_color << 8);
    cursor_x = x;
    cursor_y = y;
    uint16_t pos = x + (terminal_width * y);
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void Terminal_Class::clear() {
    for (uint8_t y = 0; y < terminal_height; y++) {
        for (uint8_t x = 0; x < terminal_width; x++) {
            frame_buffer[x + (terminal_width * y)] = ' ' | (text_color << 8);
        }
    }
    cursor_x = 0;
    cursor_y = 0;
    setCursor(0, 0);
}

void Terminal_Class::scrollUp(uint8_t n) {
    for (uint8_t i = 0; i < n; i++) {
        memcpy(&frame_buffer[0], &frame_buffer[terminal_width], terminal_width * (terminal_height - 1) * 2);
        for (uint8_t x = 0; x < terminal_width; x++) {
            frame_buffer[x + ((terminal_height - 1) * terminal_width)] = ' ' | (text_color << 8);
        }
    }
}

void Terminal_Class::scrollDown(uint8_t n) {
    for (int i = 0; i < n; i++) {
        memcpy(&frame_buffer[terminal_width], &frame_buffer[0], (terminal_width * (terminal_height - 1)) * 2);
        for (int x = 0; x < terminal_width; x++) {
            frame_buffer[x] = ' ' | (text_color << 8);
        }
    }
}

void Terminal_Class::newLine() {
    if (cursor_y == 24) {
        scrollUp(1);
        cursor_x = 0;
    }
    else {
        cursor_y++;
        cursor_x = 0;
    }
}

void Terminal_Class::showCursor(uint8_t thickness) {

    outb(0x3D4, 0x09);
    uint8_t end = inb(0x3D5) & 0b00011111; // Get the max scanline
    uint8_t start = end - thickness;

    outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | start);
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | end);
}

void Terminal_Class::hideCursor() {
    outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void Terminal_Class::OK(bool newLine) {
    print("[   ");
    char old_color = text_color;
    text_color = (text_color & 0xF0) | 0x0A;
    print("OK");
    text_color = old_color;
    if (newLine) {
        println("   ]");
    }
    else {
        print("   ]");
    }
}

void Terminal_Class::WARN(bool newLine) {
    print("[  ");
    char old_color = text_color;
    text_color = (text_color & 0xF0) | 0x06;
    print("WARN");
    text_color = old_color;
    if (newLine) {
        println("  ]");
    }
    else {
        print("  ]");
    }
}

void Terminal_Class::FAILED(bool newLine) {
    print("[ ");
    char old_color = text_color;
    text_color = (text_color & 0xF0) | 0x04;
    print("FAILED");
    text_color = old_color;
    if (newLine) {
        println(" ]");
    }
    else {
        print(" ]");
    }
}

uint8_t Terminal_Class::getWidth() {
    return terminal_width;
}

uint8_t Terminal_Class::getHeight() {
    return terminal_height;
}

uint8_t Terminal_Class::getCursorX() {
    return cursor_x;
}

uint8_t Terminal_Class::getCursorY() {
    return cursor_y;
}