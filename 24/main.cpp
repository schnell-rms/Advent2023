#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <utils.h>
#include <ratio>
#include <numeric>

using namespace std;

template<class T>
struct Point {
    T x,y,z;
};

struct Direction {
    TNumber x,y,z;
};

struct Line {
    Point<TNumber> p;
    Direction d;

    Line(const std::vector<TNumber> numbers)
    : p{numbers[0], numbers[1], numbers[2]}
    , d{numbers[3], numbers[4], numbers[5]}
    {}
};

using TLines = std::vector<Line>;

class Fraction {
public:
    Fraction(TNumber nom) : n_(nom), d_(1) {}
    
    Fraction(TNumber nom, TNumber den): n_(nom), d_(den) {
        if (d_ < 0) {
            // The sign stays with the nominator:
            d_ *= -1;
            n_ *= -1;
        }
        simplify();
    }

    bool operator==(const Fraction &other) const {
        return ((n_ == other.n_) && (d_ == other.d_));
    }

    bool operator!=(const Fraction &other) const {
        return !(operator==(other));
    }

    bool isValid() const {
        return d_ != 0;
    }

    bool operator<(const Fraction &other) {
        assert(isValid());
        assert(other.isValid());
        return n_ * other.d_ < other.n_ * d_;
    }

    bool operator<(const TNumber number) {
        assert(isValid());
        return n_ < number * d_;
    }

    Fraction operator*=(const Fraction& other)
    {
        n_ = n_ * other.n_;
        d_ = d_ * other.d_;
        simplify();
        return *this;
    }

    Fraction operator+=(const Fraction& other)
    {
        n_ = n_ * other.d_ + other.n_ * d_;
        d_ = d_ * other.d_;
        simplify();
        return *this;
    }
    
    // As a replacement to multiplication and operator<
    double asDouble() const {
        assert(isValid());
        return static_cast<double>(n_) / static_cast<double>(d_);
    }

private:
    void simplify() {
        if (d_ != 0) {
            auto a = std::gcd(n_, d_);
            d_ /= a;
            n_ /= a;
        }
    }

private:
    TNumber n_;
    TNumber d_;
};

// Brute force:
TNumber countXYIntersectionsWithTime(const TLines &lines) {
    TNumber n = 0;

    const TNumber kMinCoord = 200000000000000;
    const TNumber kMaxCoord = 400000000000000;

    auto isInArea = [kMinCoord, kMaxCoord](const Point<double> &p) {
        return  (kMinCoord <= p.x) && (p.x <= kMaxCoord) &&
                (kMinCoord <= p.y) && (p.y <= kMaxCoord);
    };

    auto intersection = [&](size_t i, size_t j) {
        Fraction tx(lines[i].p.y - lines[j].p.y, lines[j].d.y - lines[i].d.y);
        Fraction ty(lines[i].p.x - lines[j].p.x, lines[j].d.x - lines[i].d.x);

        if (!tx.isValid() || !ty.isValid() || (tx != ty)) {
            return Point<double>{kMinCoord - 1, 0, 0};
        }

        const auto &pi = lines[i].p;
        const auto &di = lines[i].d;
        return Point<double>{ pi.x + di.x * tx.asDouble(), pi.y + di.y * tx.asDouble()};//tx == ty

    };

    for(auto i=0; i<lines.size(); ++i) {
        for(auto j=i+1; j<lines.size(); ++j) {
            n += isInArea(intersection(i,j));
        }
    }

    return n;
}

// No account of time
TNumber countXYIntersections(const TLines &lines) {
    TNumber n = 0;

    const TNumber kMinCoord = 200000000000000;
    const TNumber kMaxCoord = 400000000000000;

    auto isInArea = [kMinCoord, kMaxCoord](double x, double y) {
        return  (kMinCoord <= x) && (x <= kMaxCoord) &&
                (kMinCoord <= y) && (y <= kMaxCoord);
    };

    auto isIntersection = [&](size_t i, size_t j) {
        const auto &pi = lines[i].p;
        const auto &di = lines[i].d;

        const auto &pj = lines[j].p;
        const auto &dj = lines[j].d;

        const TNumber den = di.y * dj.x - di.x * dj.y; //dj.x * di.y - di.x * dj.y;
        Fraction ti((pi.x - pj.x) * dj.y - (pi.y - pj.y) * dj.x, den);
        Fraction tj((pi.x - pj.x) * di.y - (pi.y - pj.y) * di.x, den);

        if (!ti.isValid() || !tj.isValid() || (ti<0) || (tj<0)) {
            return false;
        }

        double x = ti.asDouble() * di.x + pi.x;
        double y = ti.asDouble() * di.y + pi.y;

        if (!isInArea(x, y)) {
            return false;
        }
        
        return true;
    };

    for(auto i=0; i<lines.size(); ++i) {
        for(auto j=i+1; j<lines.size(); ++j) {
            n += isIntersection(i,j);
        }
    }

    return n;
}

int main(int argc, char *argv[]) {

    std::string inputFilePath;
    if (argc > 1) {
        inputFilePath = argv[1];
    } else {
        std::cout << "No input path" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream listFile(inputFilePath);
    if (!listFile.is_open()) {
        std::cout << "Could not open input file" << std::endl;
        return EXIT_FAILURE;
    }

    auto someLambda = [&]() {
    };
    
    std::string textLine;
    TLines geoLines;

    while(getline(listFile, textLine)) {
        if (!textLine.empty()) {
            geoLines.emplace_back(allNumbers(textLine));
        }
    }

    size_t silver = countXYIntersections(geoLines);

    cout << "Silver: " << silver << endl;

    return EXIT_SUCCESS;
}
