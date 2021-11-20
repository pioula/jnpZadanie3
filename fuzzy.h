#ifndef FUZZY_H
#define FUZZY_H
#include <iostream>
#include <tuple>
#include <cmath>
#include <set>
#include <utility>

using real_t = double;
using std::multiset;
using std::initializer_list;

namespace rank {
    using rank_t = std::tuple<real_t, real_t, real_t>;
}

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
        return u;
    }

    constexpr TriFuzzyNum& operator=(const TriFuzzyNum& that) = default;
    constexpr TriFuzzyNum& operator=(TriFuzzyNum&& that) = default;

    constexpr TriFuzzyNum& operator+=(const TriFuzzyNum& that);
    constexpr TriFuzzyNum& operator-=(const TriFuzzyNum& that);
    constexpr TriFuzzyNum& operator*=(const TriFuzzyNum& that);
    constexpr TriFuzzyNum& operator/=(const real_t& that);

    const constexpr TriFuzzyNum operator+(const TriFuzzyNum &other) const {
        return TriFuzzyNum(*this) += other;
    }

    const constexpr TriFuzzyNum operator-(const TriFuzzyNum &other) const {
        return TriFuzzyNum(*this) -= other;
    }

    const constexpr TriFuzzyNum operator*(const TriFuzzyNum &other) const {
        return TriFuzzyNum(*this) *= other;
    }

    constexpr bool operator==(const TriFuzzyNum &other) const {
        //We can depend on default implementation of "==" for tuples as
        //it compares lexicographicaly
        return calculate_rank(l, m, u) ==
               calculate_rank(other.l, other.m, other.u);
    }

    constexpr auto operator<=>(const TriFuzzyNum &other) const {
        //We can depend on default implementation of "<=>" for tuples as
        //it compares lexicographicaly
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
                                                 const real_t _u);
}; //TriFuzzyNum

constexpr rank::rank_t TriFuzzyNum::calculate_rank(const real_t _l,
                                                   const real_t _m,
                                                   const real_t _u) {
    const real_t z = (_u - _l) + std::sqrt(1 + (_u - _m) * (_u - _m))
                     + std::sqrt(1 + (_m - _l) * (_m - _l));
    const real_t x = ((_u - _l) * _m + std::sqrt(1 + (_u - _m) * (_u - _m))
                                       * _l + std::sqrt(1 + (_m - _l) * (_m - _l)) * _u) / z;
    const real_t y = (_u - _l) / z;

    return std::make_tuple(x - y / 2, 1 - y, _m);
}

constexpr void TriFuzzyNum::order_numbers(real_t &a, real_t &b, real_t &c) {
    if (a > b)
        std::swap(a, b);
    if (a > c)
        std::swap(a, c);
    if(b > c)
        std::swap(b, c);
}

constexpr TriFuzzyNum::TriFuzzyNum(real_t _l, real_t _m, real_t _u)
        : l(_l), m(_m), u(_u) {
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
constexpr TriFuzzyNum& TriFuzzyNum::operator/=(const real_t & that){
    l = l/that;
    m = m/that;
    u = u/that;
    return *this;
}
constexpr TriFuzzyNum& TriFuzzyNum::operator*=(const TriFuzzyNum& that) {
    l *= that.l;
    m *= that.m;
    u *= that.u;

    //We have to order numbers again because of multiplication of numbers
    //such as (-1, 2, 3) and (-100, 2, 3) which is (100, 4, 9)
    order_numbers(l, m, u);
    return *this;
}
consteval TriFuzzyNum crisp_number(real_t v)
{
    return TriFuzzyNum(v,v,v);
}
inline const constinit TriFuzzyNum crisp_zero = crisp_number(0);

class TriFuzzyNumSet {
private:
    multiset<TriFuzzyNum> collection;
public :
    TriFuzzyNumSet(const TriFuzzyNumSet &that) = default;
    TriFuzzyNumSet(TriFuzzyNumSet &&that) = default;
    TriFuzzyNumSet() = default;
    TriFuzzyNumSet &operator=(const TriFuzzyNumSet &t) = default;
    TriFuzzyNumSet &operator=(TriFuzzyNumSet &&t) = default;
    TriFuzzyNumSet(initializer_list<TriFuzzyNum> list) {
        this->collection = list;
    }


    void insert(TriFuzzyNum &&number) {
        collection.insert(std::move(number));
    }

    void insert(const TriFuzzyNum &number) {
        collection.insert(number);
    }


    void remove(TriFuzzyNum &number) {
        auto itr = collection.find(number);
        if (itr != collection.end()) {
            collection.erase(itr);
        }
    }

    void remove(TriFuzzyNum &&number) {
        auto itr = collection.find(number);
        if (itr != collection.end()) {
            collection.erase(itr);
        }
    }

    TriFuzzyNum arithmetic_mean() {
        if (!collection.empty()) {
            real_t counter = 0;
            TriFuzzyNum result = crisp_zero;
            for (TriFuzzyNum i : collection) {
                counter++;
                result += i;
            }
            return result/=counter;
        } else {
            throw std::length_error("TriFuzzyNumSet::arithmetic_mean - the set is empty.");
        }
    }
};
#endif //FUZZY_H