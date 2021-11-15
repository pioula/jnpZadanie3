#ifndef FUZZY_H
#define FUZZY_H
#include <iostream>
#include <tuple>
#include <cmath>

using real_t = double;

namespace rank {
    using rank_t = std::tuple<real_t, real_t, real_t>;
}

/*
To jest kod na obliczenie rank prz zminimalizowaniu używania temporaries
x1 = u - l;

real_t tmp2 = u - m;
tmp2 *= tmp2;
++tmp2;
tmp2 = std::sqrt(tmp2);

real_t tmp3 = m - l;
tmp3 *= tmp3;
++tmp3;
tmp3 = std::sqrt(tmp3);

real_t z = x1;
z += tmp2;
z += tmp3;

x2 = x1;
x2 /= z;
x2 *= (-1);

x1 *= m;
tmp2 *= l;
tmp3 *= u;

x1 += tmp2;
x1 += tmp3;
x1 /= z; //x

x1 += x2;
++x2;
x3 = m;*/

class TriFuzzyNum {
public:
    TriFuzzyNum() = delete;
    TriFuzzyNum(const TriFuzzyNum& that) = default;
    TriFuzzyNum(TriFuzzyNum&& that) = default;
    ~TriFuzzyNum() = default;
    constexpr TriFuzzyNum(real_t _l, real_t _m, real_t _u);

    constexpr real_t lower_value() const {
        return l;
    }

    constexpr real_t modal_value() const {
        return m;
    }

    constexpr real_t upper_value() const {
        return l;
    }

    constexpr TriFuzzyNum& operator=(const TriFuzzyNum& that) = default;
    constexpr TriFuzzyNum& operator=(TriFuzzyNum&& that) = default;

    constexpr TriFuzzyNum& operator+=(const TriFuzzyNum& that);
    constexpr TriFuzzyNum& operator-=(const TriFuzzyNum& that);
    constexpr TriFuzzyNum& operator*=(const TriFuzzyNum& that);

    constexpr  TriFuzzyNum operator+(const TriFuzzyNum &other) const {
        return TriFuzzyNum(*this) += other;
    }

    constexpr  TriFuzzyNum operator-(const TriFuzzyNum &other) const {
        return TriFuzzyNum(*this) -= other;
    }

    constexpr  TriFuzzyNum operator*(const TriFuzzyNum &other) const {
        return TriFuzzyNum(*this) *= other;
    }

    constexpr bool operator==(const TriFuzzyNum &other) const {
        return calculate_rank(l, m, u) == calculate_rank(other.l, other.m, other.u);
    }

    constexpr auto operator<=>(const TriFuzzyNum &other) const {
        return calculate_rank(l, m, u) <=>
            calculate_rank(other.l, other.m, other.u);
    }

private:
    real_t l, m, u;

    friend std::ostream& operator<<(std::ostream& os, const TriFuzzyNum& tfm) {
        os << "(" << tfm.l << ", " << tfm.m << ", " << tfm.u << ")";
        return os;
    }

    static constexpr void order_numbers(real_t &a, real_t &b, real_t &c);

    static constexpr rank::rank_t calculate_rank(const real_t _l,
                                                 const real_t _m,
                                                 const real_t _u) {
        real_t z = (_u - _l) + std::sqrt(1 + (_u - _m) * (_u - _m))
                   + std::sqrt(1 + (_m - _l) * (_m - _l));
        real_t x = ((_u - _l) * _m + std::sqrt(1 + (_u - _m) * (_u - _m)) * _l
                    + std::sqrt(1 + (_m - _l) * (_m - _l)) * _u) / z;
        real_t y = (_u - _l) / z;

        return std::make_tuple(x - y / 2, 1 - y, _m);
    }
};

constexpr void TriFuzzyNum::order_numbers(real_t &a, real_t &b, real_t &c) {
    if (a > b)
        std::swap(a, b);
    if (a > c)
        std::swap(a, c);
    if(b > c)
        std::swap(b, c);
}

constexpr TriFuzzyNum::TriFuzzyNum(real_t _l, real_t _m, real_t _u) {
    l = _l;
    m = _m;
    u = _u;

    order_numbers(l, m, u);
}

constexpr TriFuzzyNum& TriFuzzyNum::operator+=(const TriFuzzyNum& that) {
    l += that.l;
    m += that.m;
    u += that.u;

    return *this;
}

constexpr TriFuzzyNum& TriFuzzyNum::operator-=(const TriFuzzyNum& that) {
    l -= that.u;
    m -= that.m;
    u -= that.l;

    return *this;
}

constexpr TriFuzzyNum& TriFuzzyNum::operator*=(const TriFuzzyNum& that) {
    l *= that.l;
    m *= that.m;
    u *= that.u;

    //trzeba uporządkować, bo
    // psuje się dla -1 2 3 oraz -100 2 3 wtedy jest 100>4<9
    order_numbers(l, m, u);
    return *this;
}



#endif //FUZZY_H