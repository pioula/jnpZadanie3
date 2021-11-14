#ifndef FUZZY_H
#define FUZZY_H
#include <iostream>
#include <valarray>

typedef double real_t;

class TriFuzzyNum {
public:
    
    TriFuzzyNum() = delete;

    constexpr TriFuzzyNum(real_t _l, real_t _m, real_t _u) {
        if (_l > _m)
            std::swap(_l, _m);
        if (_l > _u)
            std::swap(_l, _u);
        if(_m > _u)
            std::swap(_l, _u);

        l = _l;
        m = _m;
        u = _u;
    }

    TriFuzzyNum(const TriFuzzyNum& that) = default;
    TriFuzzyNum(TriFuzzyNum&& that) = default;
    ~TriFuzzyNum() = default;

    constexpr real_t lower_value() const {
        return l;
    }

    constexpr real_t modal_value() const {
        return m;
    }

    constexpr real_t upper_value() const {
        return l;
    }

    TriFuzzyNum& operator=(const TriFuzzyNum& that) = default;

    TriFuzzyNum& operator=(TriFuzzyNum&& that) {
        l = std::move(that.l);
        m = std::move(that.m);
        u = std::move(that.u);
        return *this;
    }

    TriFuzzyNum& operator+=(const TriFuzzyNum& that) {
        l += that.l;
        m += that.m;
        u += that.u;

        return *this;
    }

    TriFuzzyNum& operator-=(const TriFuzzyNum& that) {
        l -= that.u;
        m -= that.m;
        u -= that.l;

        return *this;
    }

    TriFuzzyNum& operator*=(const TriFuzzyNum& that) {
        l *= that.l;
        m *= that.m;
        u *= that.m;

        return *this;
    }

    const TriFuzzyNum operator+(const TriFuzzyNum &other) const {
        return TriFuzzyNum(*this) += other;
    }

    const TriFuzzyNum operator-(const TriFuzzyNum &other) const {
        return TriFuzzyNum(*this) -= other;
    }

    const TriFuzzyNum operator*(const TriFuzzyNum &other) const {
        return TriFuzzyNum(*this) *= other;
    }

    bool operator==(const TriFuzzyNum &other) const {
        TriFuzzyNum rank_this = rank(*this);
        TriFuzzyNum rank_other = rank(other);

        return rank_this.l == rank_other.l &&
            rank_this.m == rank_other.m &&
            rank_this.u == rank_other.u;
    }

    std::strong_ordering operator<=>(const TriFuzzyNum &other) const {
        TriFuzzyNum rank_this = rank(*this);
        TriFuzzyNum rank_other = rank(other);

        if (rank_this.l > rank_other.l)
            return std::strong_ordering::less;
        else if (rank_this.l < rank_this.l)
            return std::strong_ordering::greater;
        else if (rank_this.m > rank_this.m)
            return std::strong_ordering::less;
        else if (rank_this.m < rank_this.m)
            return std::strong_ordering::greater;
        else if (rank_this.u > rank_other.u)
            return std::strong_ordering::less;
        else if (rank_this.u < rank_this.u)
            return std::strong_ordering::greater;
        else
            return std::strong_ordering::equal;
    }

private:
    real_t l, m, u;

    friend std::ostream& operator<<(std::ostream& os, const TriFuzzyNum& tfm) {
        os << "(" << tfm.l << ", " << tfm.m << ", " << tfm.u << ")";
        return os;
    }

    static TriFuzzyNum rank(const TriFuzzyNum& num) {
        real_t _l = num.lower_value();
        real_t _m = num.modal_value();
        real_t _u = num.upper_value();

        real_t tmp1 = _u - _l;

        real_t tmp2 = _u - _m;
        tmp2 *= tmp2;

        real_t tmp3 = _m - _l;
        tmp3 *= tmp3;

        tmp2 = std::sqrt(1 + tmp2);
        tmp3 = std::sqrt(1 + tmp3);

        real_t z = tmp1 + tmp2 + tmp3;
        real_t x = (tmp1 * _m + tmp2 * _l + tmp3 * _u) / z;
        real_t y = tmp1 / z;


        return {x - y / 2, 1 - y, _m};
    }
};

#endif //FUZZY_H