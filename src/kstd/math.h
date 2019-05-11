#pragma once
#include <stdint.h>

inline int floor(float value) {
    return (int)value;
}

inline int ceil(float value) {
    return floor(value) + 1;
}

inline int round(float value) {
    if ((float)(value - (int)value) > 0.5f) {
        return floor(value) + 1;
    }
    return floor(value);
}

inline int abs(int value) {
    if (value > 0) {
        return value;
    }
    else {
        return -value;
    }
}

inline float pow(float value, int power) {
    if (power > 0) {
        for (int i = 0; i < power - 1; i++) {
            value *= value;
        }
    }
    else if (power == 0) {
        return 1;
    }
    else {
        for (int i = 0; i < abs(power) + 1; i++) {
            value *= (1 / value);
        }
    }
    return value;
}

inline int pow(int value, int power) {
    if (power > 0) {
        for (int i = 0; i < power - 1; i++) {
            value *= value;
        }
    }
    else if (power == 0) {
        return 1;
    }
    else {
        for (int i = 0; i < abs(power) + 1; i++) {
            value *= (1 / value);
        }
    }
    return value;
}