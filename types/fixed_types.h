#include <bits/stdc++.h>
#include <variant>
#include <unordered_set>
#include <stdexcept>
#include <type_traits>
// #pragma once

using fixed_variants = std::variant<double, float, int64_t, int32_t, int16_t, int8_t>;

template<int N, int K>
struct Fixed {
    fixed_variants v;

    /* Конструкторы типов */

    template<typename T>
    constexpr Fixed(T value)  {
        // Проверка N
        std::unordered_set<int> allowed_N = {8, 16, 32, 64};
        if (allowed_N.find(N) == allowed_N.end()) {
            throw std::invalid_argument("not allowed N for type");
        }
        // Проверка K
        else if (K < 0 || N <= K) {
            throw std::invalid_argument("not allowed K for type");;
        }
        v = value * (1 << K);
        std::cout << value * (1 << K);
    }
    
    constexpr Fixed();

    // static constexpr Fixed from_raw(int32_t x); // задать значение без конструктора

    // // операторы
    // auto operator<=>(const Fixed&) const = default;
    // bool operator==(const Fixed&) const = default;

    // std::ostream& operator<<(std::ostream &out) {
    //     return out << v / (double)(1 << 16);
    // }

    // Fixed operator+(const Fixed& b) {
    //     return Fixed::from_raw(v + b.v);
    // }

    // Fixed operator-(const Fixed& b) {
    //     return Fixed::from_raw(v - b.v);
    // }

    // Fixed operator*(const Fixed& b) {
    //     return Fixed::from_raw(((int64_t)v * b.v) >> 16);
    // }

    // Fixed operator/(const Fixed& b) {
    //     return Fixed::from_raw(((int64_t)v << 16) / b.v);
    // }

    // Fixed& operator+=(const Fixed& b) {
    //     *this = *this + b;
    //     return *this;
    // }

    // Fixed& operator-=(const Fixed& b) {
    //     *this = *this - b;
    //     return *this;
    // }

    // Fixed& operator*=(const Fixed& b) {
    //     *this = *this * b;
    //     return *this;
    // }

    // Fixed& operator/=(const Fixed& b) {
    //     *this = *this / b;
    //     return *this;
    // }

    // Fixed operator-() const {
    //     return Fixed::from_raw(-v);
    // }
};

// Fixed abs(Fixed x); // значение по модулю

int main(void) {
    std::cout << 5;
    Fixed<64, 5> value(5);
}