#ifndef ADVENT2023_FRACTION_H
#define ADVENT2023_FRACTION_H

#include <numeric>

template <class T>
class Fraction {
public:
    constexpr Fraction(T nom) : n_(nom), d_(1) {}
    
    constexpr Fraction(T nom, T den): n_(nom), d_(den) {
        if (d_ < 0) {
            // The sign stays with the nominator:
            d_ *= -1;
            n_ *= -1;
        }
        simplify();
    }

    bool isValid() const {
        return d_ != 0;
    }

    bool operator<(const Fraction &other) const {
        assert(isValid());
        assert(other.isValid());
        return n_ * other.d_ < other.n_ * d_;
    }

    bool operator<(const T number) const {
        assert(isValid());
        return n_ < number * d_;
    }

    Fraction& operator+=(const Fraction& other)
    {
        n_ = n_ * other.d_ + other.n_ * d_;
        d_ = d_ * other.d_;
        simplify();
        return *this;
    }

    Fraction& operator-=(const Fraction& other)
    {
        return operator+=(Fraction{-other.n_, other.d_});
    }

    Fraction& operator*=(const Fraction& other)
    {
        n_ = n_ * other.n_;
        d_ = d_ * other.d_;
        simplify();
        return *this;
    }

    Fraction& operator/=(const Fraction& other)
    {
        return operator*=(Fraction{other.d_, other.n_});
    }
    
    // As a replacement to all operators
    double asDouble() const {
        assert(isValid());
        return static_cast<double>(n_) / static_cast<double>(d_);
    }

    T num() const { return n_; }
    T den() const { return d_; }
    
private:
    void simplify() {
        if (d_ != 0) {
            auto a = std::gcd(n_, d_);
            d_ /= a;
            n_ /= a;
        }
    }

private:
    T n_;
    T d_;
};

template <class T>
std::ostream& operator<<(std::ostream& out, const Fraction<T>& f)
{
    out << f.num();
    return (f.den() != 1) ? (out << '/' << f.den()) : out;
}

template <class T>
constexpr bool operator==(const Fraction<T>& lhs, Fraction<T>& rhs)
{
    return lhs.num() == rhs.num() && lhs.den() == rhs.den();
}

template <class T>
constexpr bool operator==(const Fraction<T>& lhs, const Fraction<T>& rhs)
{
    return lhs.num() == rhs.num() && lhs.den() == rhs.den();
}

template <class T> 
constexpr bool operator!=(const Fraction<T>& lhs, const Fraction<T>& rhs)
{
    return !(lhs == rhs);
}

template <class T>
constexpr Fraction<T> operator+(Fraction<T> lhs, const Fraction<T>& rhs)
{
    return lhs += rhs;
}

template <class T>
constexpr Fraction<T> operator-(Fraction<T> lhs, const Fraction<T>& rhs)
{
    return lhs -= rhs;
}

template <class T>
constexpr Fraction<T> operator*(Fraction<T> lhs, const Fraction<T>& rhs)
{
    return lhs *= rhs;
}

template <class T>
constexpr Fraction<T> operator/(Fraction<T> lhs, const Fraction<T>& rhs)
{
    return lhs /= rhs;
}

#endif // ADVENT2023_FRACTION_H
