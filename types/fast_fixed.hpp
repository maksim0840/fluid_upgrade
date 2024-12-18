#include "base.hpp"
#pragma once

template<int N, int K>
struct FastFixed {
    using variants = std::variant<double, float, IntFast64, IntFast32, IntFast16, IntFast8, int64_t, int32_t, int16_t, int8_t>;
    variants v_fastfixed;

    // Получить v_fastfixed
    const variants& get_v() const {
        return v_fastfixed;
    }

    /* Конструкторы типов */

    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    FastFixed(T value, double accuracy_value = 0.0) {
        // Проверка K
        if (K < 0 || N <= K) {
            throw std::invalid_argument("not allowed K for type");
        }
        // Проверка совместимости размера типа и переданного N
        if (IS_SAME_V_FLOAT) {
            if (N == sizeof(float)*CHAR_BIT) v_fastfixed = static_cast<float>(value * (1 << K));
            else if (N == sizeof(double)*CHAR_BIT) v_fastfixed = static_cast<double>(value * (1 << K));
            else throw std::invalid_argument("not allowed N for float types");
        }
        else if (IS_SAME_V_INT || IS_SAME_V_FAST_INT) {
            if (accuracy_value == 0.0) {
                if (1 <= N && N <= 8) v_fastfixed = IntFast8{static_cast<int_fast8_t>(value * (1 << K))};
                else if (9 <= N && N <= 16) v_fastfixed = IntFast16{static_cast<int_fast16_t>(value * (1 << K))};
                else if (17 <= N && N <= 32) v_fastfixed = IntFast32{static_cast<int_fast32_t>(value * (1 << K))};
                else if (33 <= N && N <= 64) v_fastfixed = IntFast64{static_cast<int_fast64_t>(value * (1 << K))};
                else throw std::invalid_argument("not allowed N for int types");
            }
            else {
                if (1 <= N && N <= 8) v_fastfixed = IntFast8{static_cast<int_fast8_t>(accuracy_value * (1 << K))};
                else if (9 <= N && N <= 16) v_fastfixed = IntFast16{static_cast<int_fast16_t>(accuracy_value * (1 << K))};
                else if (17 <= N && N <= 32) v_fastfixed = IntFast32{static_cast<int_fast32_t>(accuracy_value * (1 << K))};
                else if (33 <= N && N <= 64) v_fastfixed = IntFast64{static_cast<int_fast64_t>(accuracy_value * (1 << K))};
                else throw std::invalid_argument("not allowed N for int types");
            }
        }
        else {
            throw std::invalid_argument("unsuported type");
        }
    }

    // Приведение других кастомных типов к текущему
    template<template<int, int> class FixType1, int N1, int K1>
    FastFixed(FixType1<N1, K1> other) {
        v_fastfixed = std::visit([this](auto&& v_value) -> FastFixed{
            return this->from_raw(unpack(v_value) / (1 << K1));
        }, other.get_v()).get_v();
    }

    FastFixed() : FastFixed(0.0) {} // double = 0 по умолчанию

     // Создаёт объект Fixed из сырого значения, преобразуя его в тип текущего variant
    template<typename T>
    FastFixed from_raw(T other_value) const {
        // std::visit для перебора и нахождения значения с текущим типом хранящимся в std::variant
        return std::visit([other_value](auto&& v_value) -> FastFixed{
            using ValueType = std::decay_t<decltype(unpack(v_value))>; // берём тип значения ТЕКУЩЕГО объекта
            double accuracy_value = unpack(other_value);
            FastFixed<N, K> buf(static_cast<ValueType>(accuracy_value), accuracy_value); // присваиваем такой же тип НОВОМУ значения для НОВОГО объекта
            return buf;
        }, get_v());
    }
};