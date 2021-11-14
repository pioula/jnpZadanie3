#ifndef FUZZY_H
#define FUZZY_H
#include <iostream>
#include <valarray>

typedef double real_t;

namespace rank {
    class TriFuzzyRank {
    public:
        TriFuzzyRank() = default;
        TriFuzzyRank(const TriFuzzyRank& that) = default;
        TriFuzzyRank(TriFuzzyRank&& that) = default;
        ~TriFuzzyRank() = default;
        TriFuzzyRank(real_t l, real_t m, real_t u) {
            //To wygląda jak totalny syf, ale to co robi to liczy rangę dla TriFuzzyLiczby
            //W tej czytance jest napisane, że tak trzeba robić:
            //https://web.archive.org/web/20160709175149/http://www.informit.com/guides/content.aspx?g=cplusplus&seqNum=198
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
            x3 = m;
        }

        bool operator==(const TriFuzzyRank &other) const;
        std::partial_ordering operator<=>(const TriFuzzyRank &other) const;
    private:
        real_t x1, x2, x3;
    }; //TriFuzzyRank

    bool TriFuzzyRank::operator==(const TriFuzzyRank &other) const {
        return x1 == other.x1 &&
                x2 == other.x2 &&
                x3 == other.x3;
    }

    std::partial_ordering
    TriFuzzyRank::operator<=>(const TriFuzzyRank &other) const {
        return x1 != other.x1 ? x1 <=> other.x1 :
            x2 != other.x2 ? x2 <=> other.x2 :
                x3 <=> other.x3;
    }
}//rank

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

    TriFuzzyNum& operator=(const TriFuzzyNum& that) = default;
    TriFuzzyNum& operator=(TriFuzzyNum&& that) = default;

    TriFuzzyNum& operator+=(const TriFuzzyNum& that);
    TriFuzzyNum& operator-=(const TriFuzzyNum& that);
    TriFuzzyNum& operator*=(const TriFuzzyNum& that);

    const TriFuzzyNum operator+(const TriFuzzyNum &other) const {
        return TriFuzzyNum(*this) += other;
    }

    const TriFuzzyNum operator-(const TriFuzzyNum &other) const {
        return TriFuzzyNum(*this) -= other;
    }

    const TriFuzzyNum operator*(const TriFuzzyNum &other) const {
        return TriFuzzyNum(*this) *= other;
    }

    bool operator==(const TriFuzzyNum &other) const { //to jest jakieś nieefektywne, ale sqrt nie jest constexpr
        return rank::TriFuzzyRank(l, m, u) ==
            rank::TriFuzzyRank(other.l, other.m, other.u);
    }

    std::partial_ordering operator<=>(const TriFuzzyNum &other) const {//same ^
        return rank::TriFuzzyRank(l, m, u) <=>
               rank::TriFuzzyRank(other.l, other.m, other.u);
    }

private:
    real_t l, m, u;

    friend std::ostream& operator<<(std::ostream& os, const TriFuzzyNum& tfm) {
        os << "(" << tfm.l << ", " << tfm.m << ", " << tfm.u << ")";
        return os;
    }

    static constexpr void order_numbers(real_t &a, real_t &b, real_t &c);
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

TriFuzzyNum& TriFuzzyNum::operator+=(const TriFuzzyNum& that) {
    l += that.l;
    m += that.m;
    u += that.u;

    return *this;
}

TriFuzzyNum& TriFuzzyNum::operator-=(const TriFuzzyNum& that) {
    l -= that.u;
    m -= that.m;
    u -= that.l;

    return *this;
}

TriFuzzyNum& TriFuzzyNum::operator*=(const TriFuzzyNum& that) {
    l *= that.l;
    m *= that.m;
    u *= that.u;

    //trzeba uporządkować, bo
    // psuje się dla -1 2 3 oraz -100 2 3 wtedy jest 100>4<9
    order_numbers(l, m, u);
    return *this;
}

#endif //FUZZY_H