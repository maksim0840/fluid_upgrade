#include <bits/stdc++.h>
#include <variant>
#include <unordered_set>
#include <stdexcept>
#include <type_traits>
#include <climits>
#include <cstdint>
#define EPS 0.000001
#pragma once

#define IS_SAME_AS_FLOAT std::is_same_v<T, double> || std::is_same_v<T, float>
#define IS_SAME_AS_INT std::is_same_v<T, int64_t> || std::is_same_v<T, int32_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int8_t>


template<int N, int K>
struct Fixed {
    using variants = std::variant<double, float, int64_t, int32_t, int16_t, int8_t>;
    variants v;

    /* Конструкторы типов */

    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr Fixed(T value)  {
        // Множество размеров доступных типов (в битах)
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

    // Создаёт объект Fixed из сырого значения, преобразуя его в тип текущего variant
    template<typename T>
    constexpr Fixed from_raw(T other_value) {
        // std::visit для перебора и нахождения значения с текущим типом хранящимся в std::variant
        return std::visit([other_value](auto&& v_value) -> Fixed{
            using ValueType = std::decay_t<decltype(v_value)>; // берём тип значения ТЕКУЩЕГО объекта
            Fixed<N, K> buf(static_cast<ValueType>(other_value)); // присваиваем такой же тип НОВОМУ значения для НОВОГО объекта
            return buf;
        }, v);
    }

    // Берёт значение из variant и преобразует его в нужный тип T
    template <typename T>
    static T get_value_by_type(const variants& v) {
        return std::visit([](auto&& v_value) -> T{
            return static_cast<T>(v_value);
        }, v);
    }

    /* Операторы */

    template<int otherN, int otherK>
    auto operator<=>(const Fixed<otherN, otherK>& b) const {
        double value1 = get_value_by_type<double>(v); // приводим левое значение к double
        double value2 = get_value_by_type<double>(b.v); // приводим правое значение к double
        return value1 <=> value2;
    }
    
    template<int otherN, int otherK>
    bool operator==(const Fixed<otherN, otherK>& b) const {
        double value1 = get_value_by_type<double>(v); // приводим левое значение к double
        double value2 = get_value_by_type<double>(b.v); // приводим правое значение к double
        return std::abs(value1 - value2) < EPS;
    }

    template<int otherN, int otherK>
    Fixed operator+(const Fixed<otherN, otherK>& b) {
        return std::visit([this, &b](auto&& v_value) -> Fixed{
            using MainType = std::decay_t<decltype(v_value)>; // тип левого значения
            MainType other_value = get_value_by_type<MainType>(b.v); // приводим правое значение к типу MainType
            Fixed<N, K> buf = this->from_raw(v_value + other_value); // создаём Fixed с типом MainType и результатом операции
            return buf;
        }, v);
    }

    template<int otherN, int otherK>
    Fixed operator-(const Fixed<otherN, otherK>& b) {
        return std::visit([this, &b](auto&& v_value) -> Fixed{
            using MainType = std::decay_t<decltype(v_value)>; // тип левого значения
            MainType other_value = get_value_by_type<MainType>(b.v); // приводим правое значение к типу MainType
            Fixed<N, K> buf = this->from_raw(v_value - other_value); // создаём Fixed с типом MainType и результатом операции
            return buf;
        }, v);
    }

    template<int otherN, int otherK>
    Fixed operator*(const Fixed<otherN, otherK>& b) {
        double value1 = get_value_by_type<double>(v); // приводим левое значение к double
        double value2 = get_value_by_type<double>(b.v); // приводим правое значение к double
        Fixed<N, K> buf = this->from_raw(value1 * value2); // создаём Fixed с типом левого значения и результатом операции
        return buf;
    }

    template<int otherN, int otherK>
    Fixed operator/(const Fixed<otherN, otherK>& b) {
        double value1 = get_value_by_type<double>(v); // приводим левое значение к double
        double value2 = get_value_by_type<double>(b.v); // приводим правое значение к double
        Fixed<N, K> buf = this->from_raw(value1 / value2); // создаём Fixed с типом левого значения и результатом операции
        return buf;
    }

    template<int otherN, int otherK>
    Fixed operator+=(const Fixed<otherN, otherK>& b) {
        *this = *this + b;
        return *this;
    }

    template<int otherN, int otherK>
    Fixed operator-=(const Fixed<otherN, otherK>& b) {
        *this = *this - b;
        return *this;
    }

    template<int otherN, int otherK>
    Fixed operator*=(const Fixed<otherN, otherK>& b) {
        *this = *this * b;
        return *this;
    }

    template<int otherN, int otherK>
    Fixed operator/=(const Fixed<otherN, otherK>& b) {
        *this = *this / b;
        return *this;
    }

    Fixed operator-() {
        return std::visit([this](auto&& v_value) -> Fixed{
            Fixed<N, K> buf = this->from_raw(-v_value);
            return buf;
        }, v);
    }
    
    friend std::ostream& operator<<(std::ostream &out, const Fixed<N, K>& obj) {
        return std::visit([&out](auto&& v_value) -> std::ostream&{
            return out << v_value / (double)(1 << K);
        }, obj.v);
    }
};