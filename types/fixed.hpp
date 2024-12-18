#include "base.hpp"
#pragma once

template<int N, int K>
struct Fixed {
    using variants = std::variant<double, float, int64_t, int32_t, int16_t, int8_t>;
    variants v_fixed;
    
    // Получить v_fixed
    const variants& get_v() const {
        return v_fixed;
    }

    /* Конструкторы типов */

    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    Fixed(T value, double accuracy_value = 0.0)  {
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
        if constexpr(IS_SAME_V_FLOAT) {
            if (N == sizeof(float)*CHAR_BIT) v_fixed = static_cast<float>(value * (1 << K));
            else if (N == sizeof(double)*CHAR_BIT) v_fixed = static_cast<double>(value * (1 << K));
            else throw std::invalid_argument("not allowed N for float types");
        }
        else if constexpr(IS_SAME_V_INT) {
            if (accuracy_value == 0.0) {
                if (N == 8) v_fixed = static_cast<int8_t>(value * (1 << K));
                else if (N == 16) v_fixed = static_cast<int16_t>(value * (1 << K));
                else if (N == 32) v_fixed = static_cast<int32_t>(value * (1 << K));
                else if (N == 64) v_fixed = static_cast<int64_t>(value * (1 << K));
                else throw std::invalid_argument("not allowed N for int types");
            }
            else {
                if (N == 8) v_fixed = static_cast<int8_t>(accuracy_value * (1 << K));
                else if (N == 16) v_fixed = static_cast<int16_t>(accuracy_value * (1 << K));
                else if (N == 32) v_fixed = static_cast<int32_t>(accuracy_value * (1 << K));
                else if (N == 64) v_fixed = static_cast<int64_t>(accuracy_value * (1 << K));
                else throw std::invalid_argument("not allowed N for int types");
            }
        }
        else {
            throw std::invalid_argument("unsuported type");
        }
    }
    
    // Приведение других кастомных типов к текущему
    template<template<int, int> class FixType1, int N1, int K1>
    Fixed(FixType1<N1, K1> other) {
        v_fixed = std::visit([this](auto&& v_value) -> Fixed{
            return this->from_raw(unpack(v_value) / (1 << K1));
        }, other.get_v()).get_v();
    }

    Fixed() : Fixed(0.0) {} // double = 0 по умолчанию

    // Создаёт объект Fixed из сырого значения, преобразуя его в тип текущего variant
    template<typename T>
    Fixed from_raw(T other_value) const {
        // std::visit для перебора и нахождения значения с текущим типом хранящимся в std::variant
        return std::visit([other_value](auto&& v_value) -> Fixed{
            using ValueType = std::decay_t<decltype(unpack(v_value))>; // берём тип значения ТЕКУЩЕГО объекта
            double accuracy_value = unpack(other_value);
            Fixed<N, K> buf(static_cast<ValueType>(accuracy_value), accuracy_value); // присваиваем такой же тип НОВОМУ значения для НОВОГО объекта
            return buf;
        }, get_v());
    }
};