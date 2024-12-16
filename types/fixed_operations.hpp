#include "fixed.hpp"
#include "fast_fixed.hpp"
#pragma once

#define EPS 0.000001


template<template<int, int> class FixType1, int N1, int K1, template<int, int> class FixType2, int N2, int K2>
std::partial_ordering operator<=>(const FixType1<N1, K1>& a, const FixType2<N2, K2>& b) {
    double value1 = get_value_as_double(a.get_v());
    double value2 = get_value_as_double(b.get_v());
    if (std::abs(value1 - value2) < EPS) {
        return std::partial_ordering::equivalent; // если значения равны
    }
    return value1 <=> value2; // сравнение
}

template<template<int, int> class FixType1, int N1, int K1, template<int, int> class FixType2, int N2, int K2>
bool operator==(FixType1<N1, K1> a, FixType2<N2, K2> b) {
    double value1 = get_value_as_double(a.get_v());
    double value2 = get_value_as_double(b.get_v());
    return std::abs(value1 - value2) < EPS;
}

template<template<int, int> class FixType1, int N1, int K1, template<int, int> class FixType2, int N2, int K2>
FixType1<N1, K1> operator+(FixType1<N1, K1> a, FixType2<N2, K2> b) {
    double value1 = get_value_as_double(a.get_v());
    double value2 = get_value_as_double(b.get_v());
    FixType1<N1, K1> buf = a.from_raw(value1 / (1 << K1) + value2 / (1 << K2));
    return buf;
}

template<template<int, int> class FixType1, int N1, int K1, template<int, int> class FixType2, int N2, int K2>
FixType1<N1, K1> operator-(FixType1<N1, K1> a, FixType2<N2, K2> b) {
    double value1 = get_value_as_double(a.get_v());
    double value2 = get_value_as_double(b.get_v());
    FixType1<N1, K1> buf = a.from_raw(value1 / (1 << K1) - value2 / (1 << K2));
    return buf;
}

template<template<int, int> class FixType1, int N1, int K1, template<int, int> class FixType2, int N2, int K2>
FixType1<N1, K1> operator*(FixType1<N1, K1> a, FixType2<N2, K2> b) {
    double value1 = get_value_as_double(a.get_v());
    double value2 = get_value_as_double(b.get_v());
    int koef = (K1 >= K2) ? 1 : -1;
    FixType1<N1, K1> buf = a.from_raw(koef * value1 / (1 << K1) * value2 / (1 << K2));
    return buf;
}

template<template<int, int> class FixType1, int N1, int K1, template<int, int> class FixType2, int N2, int K2>
FixType1<N1, K1> operator/(FixType1<N1, K1> a, FixType2<N2, K2> b) {
    double value1 = get_value_as_double(a.get_v());
    double value2 = get_value_as_double(b.get_v());
    int koef = (K1 >= K2) ? 1 : -1;
    FixType1<N1, K1> buf = a.from_raw(koef * (value1 / (1 << K1)) / (value2 / (1 << K2)));
    return buf;
}

template<template<int, int> class FixType1, int N1, int K1, template<int, int> class FixType2, int N2, int K2>
FixType1<N1, K1> operator+=(FixType1<N1, K1>& a, FixType2<N2, K2> b) {
    a = a + b;
    return a;
}

template<template<int, int> class FixType1, int N1, int K1, template<int, int> class FixType2, int N2, int K2>
FixType1<N1, K1> operator-=(FixType1<N1, K1>& a, FixType2<N2, K2> b) {
    a = a - b;
    return a;
}

template<template<int, int> class FixType1, int N1, int K1, template<int, int> class FixType2, int N2, int K2>
FixType1<N1, K1> operator*=(FixType1<N1, K1>& a, FixType2<N2, K2> b) {
    a = a * b;
    return a;
}

template<template<int, int> class FixType1, int N1, int K1, template<int, int> class FixType2, int N2, int K2>
FixType1<N1, K1> operator/=(FixType1<N1, K1>& a, FixType2<N2, K2> b) {
    a = a / b;
    return a;
}

template<template<int, int> class FixType1, int N1, int K1>
FixType1<N1, K1> operator-(FixType1<N1, K1>& a) {
    double value1 = get_value_as_double(a.get_v());
    FixType1<N1, K1> buf = a.from_raw(-unpack(value1) / (1 << K1)); 
    return buf;
}

template<template<int, int> class FixType1, int N1, int K1>
std::ostream& operator<<(std::ostream &out, const FixType1<N1, K1>& a) {
    return std::visit([&out](auto&& v_value) -> std::ostream& {
        return out << unpack(v_value) / (1 << K1);
    }, a.get_v());
}