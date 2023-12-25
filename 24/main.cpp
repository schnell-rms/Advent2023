#include <iostream>
#include <sstream>
#include <fstream>
#include "z3++.h"

#include <string>
#include <utils.h>
#include <ratio>
#include <numeric>

#include <fraction.h>

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

// Gold: solve for the first 3 trajectories. The solution should fit all.
TNumber solveGoldEquation(const TLines &lines) {
/*
Equations are in the form of (where s is the start coordinate of the rock and v is the speed):
p0.x + d0.x * t0 = sx + vx * t0
p0.y + d0.y * t0 = sy + vy * t0
p0.z + d0.z * t0 = sz + vz * t0

p1.x + d1.x * t1 = sx + vx * t1
p1.y + d1.y * t1 = sy + vy * t1
p1.z + d1.z * t1 = sz + vz * t1

p2.x + d2.x * t2 = sx + vx * t2
p2.y + d2.y * t2 = sy + vy * t2
p2.z + d2.z * t2 = sz + vz * t2

Nine equations, 9 unkonws (3 t#, 3 s# and 3 v#), should be enough and should match the solution for the given input.
Solve this using an equation solver utility!!!
*/
    z3::context ctx;
    z3::expr    sx = ctx.int_const("sx");
    z3::expr    sy = ctx.int_const("sy");
    z3::expr    sz = ctx.int_const("sz");
    z3::expr    vx = ctx.int_const("vx");
    z3::expr    vy = ctx.int_const("vy");
    z3::expr    vz = ctx.int_const("vz");
    z3::solver  solver(ctx);

    for (int i = 0; i < 3; ++i)
    {
      auto oneLine = lines[i];
      string strT = "t" + to_string(i);

      z3::expr t = ctx.int_const(strT.c_str());
      auto px = ctx.int_val(oneLine.p.x);
      auto dx = ctx.int_val(oneLine.d.x);

      auto py = ctx.int_val(oneLine.p.y);
      auto dy = ctx.int_val(oneLine.d.y);

      auto pz = ctx.int_val(oneLine.p.z);
      auto dz = ctx.int_val(oneLine.d.z);

      solver.add(px + t * dx == sx + t * vx);
      solver.add(py + t * dy == sy + t * vy);
      solver.add(pz + t * dz == sz + t * vz);
    }

    solver.check();
    auto solution = solver.get_model();

    if (gIS_DEBUG) {
        cout << "Start x " << solution.eval(sx) << endl;
        cout << "Start y " << solution.eval(sy) << endl;
        cout << "Start z " << solution.eval(sz) << endl;

        cout << "Speed x " << solution.eval(vx) << endl;
        cout << "Speed y " << solution.eval(vy) << endl;
        cout << "Speed z " << solution.eval(vz) << endl;
    }

    return std::stoll(solution.eval(sx + sy + sz).to_string());
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

    std::string textLine;
    TLines geoLines;

    while(getline(listFile, textLine)) {
        if (!textLine.empty()) {
            geoLines.emplace_back(allNumbers(textLine));
        }
    }

    const size_t silver = countXYIntersections(geoLines);
    cout << "Silver: " << silver << endl;

    const auto goldz3 = solveGoldEquation(geoLines);
    cout << "GOLD Z3: " << goldz3 << endl;

    return EXIT_SUCCESS;
}
