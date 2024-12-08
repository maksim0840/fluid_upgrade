#include <bits/stdc++.h>
#pragma once

// вещественное число с фиксированной точкой
struct Fixed {
    int32_t v;

    // конструкторы типов
    constexpr Fixed(int v): v(v << 16) {}
    constexpr Fixed(float f): v(f * (1 << 16)) {}
    constexpr Fixed(double f): v(f * (1 << 16)) {}
    constexpr Fixed(): v(0) {}

    static constexpr Fixed from_raw(int32_t x); // задать значение без конструктора

    // операторы
    auto operator<=>(const Fixed&) const = default;
    bool operator==(const Fixed&) const = default;
    std::ostream& operator<<(std::ostream &out);
    Fixed operator+(const Fixed& b);
    Fixed operator-(const Fixed& b);
    Fixed operator*(const Fixed& b);
    Fixed operator/(const Fixed& b);
    Fixed& operator+=(const Fixed& b);
    Fixed& operator-=(const Fixed& b);
    Fixed& operator*=(const Fixed& b);
    Fixed& operator/=(const Fixed& b);
    Fixed operator-() const;
};

Fixed abs(Fixed x); // значение по модулю
