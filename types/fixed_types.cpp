#include "fixed_types.h"

constexpr Fixed Fixed::from_raw(int32_t x) {
    Fixed ret;
    ret.v = x;
    return ret;
} 

std::ostream& Fixed::operator<<(std::ostream &out) {
    return out << v / (double)(1 << 16);
}

Fixed Fixed::operator+(const Fixed& b) {
    return Fixed::from_raw(v + b.v);
}

Fixed Fixed::operator-(const Fixed& b) {
    return Fixed::from_raw(v - b.v);
}

Fixed Fixed::operator*(const Fixed& b) {
    return Fixed::from_raw(((int64_t)v * b.v) >> 16);
}

Fixed Fixed::operator/(const Fixed& b) {
    return Fixed::from_raw(((int64_t)v << 16) / b.v);
}

Fixed& Fixed::operator+=(const Fixed& b) {
    *this = *this + b;
    return *this;
}

Fixed& Fixed::operator-=(const Fixed& b) {
    *this = *this - b;
    return *this;
}

Fixed& Fixed::operator*=(const Fixed& b) {
    *this = *this * b;
    return *this;
}

Fixed& Fixed::operator/=(const Fixed& b) {
    *this = *this / b;
    return *this;
}

Fixed Fixed::operator-() const {
    return Fixed::from_raw(-v);
}

Fixed abs(Fixed x) {
    if (x.v < 0) {
        x.v = -x.v;
    }
    return x;
}