#include "base.hpp"
#pragma once

template<int N, int K>
struct Float {
    using variants = std::variant<double, float>;
    variants v_float;
    
    // Получить v_float
    const variants& get_v() const {
        return v_float;
    }

    /* Конструкторы типов */

    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    Float(T value, double accuracy_value = 0.0)  {
        // Проверка K
        if (K != 0) {
            //throw std::invalid_argument("not allowed K for type");
        }
        // Проверка N
        if (!((N == sizeof(float)*CHAR_BIT) || (N == sizeof(double)*CHAR_BIT))) {
            throw std::invalid_argument("not allowed N for type");
        }
        // Проверка совместимости размера типа и переданного N
        if (N == sizeof(float)*CHAR_BIT) v_float = static_cast<float>(value);
        else if (N == sizeof(double)*CHAR_BIT) v_float = static_cast<double>(value);
        else {
            throw std::invalid_argument("unsuported type");
        }
    }
    
    // Приведение других кастомных типов к текущему
    template<template<int, int> class FixType1, int N1, int K1>
    Float(FixType1<N1, K1> other) {
        v_float = std::visit([this](auto&& v_value) -> Float{
            return this->from_raw(unpack(v_value) / (1 << K1));
        }, other.get_v()).get_v();
    }

    Float() : Float(0.0) {} // double = 0 по умолчанию

    // Создаёт объект Float из сырого значения, преобразуя его в тип текущего variant
    template<typename T>
    Float from_raw(T other_value) const {
        // std::visit для перебора и нахождения значения с текущим типом хранящимся в std::variant
        return std::visit([other_value](auto&& v_value) -> Float{
            using ValueType = std::decay_t<decltype(unpack(v_value))>; // берём тип значения ТЕКУЩЕГО объекта
            double accuracy_value = unpack(other_value);
            Float<N, K> buf(static_cast<ValueType>(accuracy_value), accuracy_value); // присваиваем такой же тип НОВОМУ значения для НОВОГО объекта
            return buf;
        }, get_v());
    }
};