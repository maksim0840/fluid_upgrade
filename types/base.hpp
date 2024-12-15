#include <bits/stdc++.h>
#include <variant>
#include <unordered_set>
#include <stdexcept>
#include <type_traits>
#include <climits>
#include <cstdint>
#include <iostream>
#include <compare>

#pragma once

#define IS_SAME_V_FLOAT std::is_same_v<T, double> || std::is_same_v<T, float>
#define IS_SAME_V_INT std::is_same_v<T, int64_t> || std::is_same_v<T, int32_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int8_t>
#define IS_SAME_V_FAST_INT std::is_same_v<T, int_fast64_t> || std::is_same_v<T, int_fast32_t> || std::is_same_v<T, int_fast16_t> || std::is_same_v<T, int_fast8_t>

/* Добавляем дополнительную обёртку для fast типов т.к. они могут ссылаться на один и тот
же базовый тип, что приводит к конфликту значений в std::variant */
struct IntFast8 { int_fast8_t value; };
struct IntFast16 { int_fast16_t value; };
struct IntFast32 { int_fast32_t value; };
struct IntFast64 { int_fast64_t value; };

// распаковка типов (если вдруг они являются обёрктой)
int_fast8_t unpack(const IntFast8& v) { return v.value; }
int_fast16_t unpack(const IntFast16& v) { return v.value; }
int_fast32_t unpack(const IntFast32& v) { return v.value; }
int_fast64_t unpack(const IntFast64& v) { return v.value; }
template<typename OtherType>
OtherType unpack(const OtherType& v) {
    return v;
}

// получить текущее значение из std::variant как тип double 
template <typename Variant>
double get_value_as_double(const Variant& v) {
    return std::visit([](auto&& v_value) -> double{
        return static_cast<double>(unpack(v_value));
    }, v);
}