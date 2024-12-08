#include <bits/stdc++.h>
#include <variant>
#include <unordered_set>
#include <stdexcept>
#include <type_traits>
#include <climits>

// #pragma once

#define IS_SAME_AS_FLOAT std::is_same_v<T, double> || std::is_same_v<T, float>
#define IS_SAME_AS_INT std::is_same_v<T, int64_t> || std::is_same_v<T, int32_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int8_t>


template<int N, int K>
struct Fixed {
    std::variant<double, float, int64_t, int32_t, int16_t, int8_t> v;

    /* Конструкторы типов */

    template<typename T>
    constexpr Fixed(T value)  {
        // множество размеров доступных типов (в битах)
        std::unordered_set<int> allowed_N = {sizeof(float)*CHAR_BIT, sizeof(double)*CHAR_BIT, 8, 16, 32, 64};
        // Проверка K
        if (K < 0 || N <= K) {
            throw std::invalid_argument("not allowed K for type");
        }
        // Проверка N
        if (allowed_N.find(N) == allowed_N.end()) {
            throw std::invalid_argument("not allowed N for type");
        }
        // Проверка совместимости размера типа и переданного N
        if constexpr(IS_SAME_AS_FLOAT) {
            if (N == sizeof(float)*CHAR_BIT) v = static_cast<float>(value * (1 << K));
            else if (N == sizeof(double)*CHAR_BIT) v = static_cast<double>(value * (1 << K));
            else throw std::invalid_argument("not allowed N for float types");
        }
        else if constexpr(IS_SAME_AS_INT) {
            if (N == 8) v = static_cast<int8_t>(value * (1 << K));
            else if (N == 16) v = static_cast<int16_t>(value * (1 << K));
            else if (N == 32) v = static_cast<int32_t>(value * (1 << K));
            else if (N == 64) v = static_cast<int64_t>(value * (1 << K));
            else throw std::invalid_argument("not allowed N for int types");
        }
        else {
            throw std::invalid_argument("unsuported type");
        }
    }
    
    constexpr Fixed() : Fixed(0.0) {} // double = 0 по умолчанию

    template<typename T>
    constexpr Fixed from_raw(T x) {
        // std::visit для перебора и нахождения значения с текущим типом хранящимся в std::variant
        std::visit([](auto&& value, T x) {
            using ValueType = std::decay_t<decltype(v)>; // берём тип значения ТЕКУЩЕГО объекта
            Fixed<N, K> buf(static_cast<ValueType>(x)); // присваиваем такой же тип НОВОМУ значения для НОВОГО объекта
            return buf;
        }, v, x);
    }

    /* Операторы */

    auto operator<=>(const Fixed&) const = default;
    bool operator==(const Fixed&) const = default;

    // std::ostream& operator<<(std::ostream &out) {
    //     return out << v / (double)(1 << 16);
    // }

    Fixed operator+(const Fixed& b) {
        // ВЗЯТЬ ТЕКУЩЕЕ ЗНАЧЕНИЕ, ПЕРЕДЕЛАТЬ В ДАБЛ, СЛОЖИТЬ, СОЗДАТЬ НОВЫЙ ОБЪЕКТ С ИЗНАЧАЛЬНЫМ ТИПОМ, КАСТАНУТЬ ПОЛУЧЕННОЕ СЛОЖЕНИЕ ДАБЛОВ К НОВОМУ ОБЪЕКТУ
        return from_raw(static_cast<double>(v) + static_cast<double>(b.v));
    }

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

    Fixed<32, 0> value(5);
    return 0;
}