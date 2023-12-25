#include <iostream>
#include <sstream>
#include <fstream>
#include "z3++.h"

#include <string>
#include <utils.h>
#include <ratio>
#include <numeric>

#include <fraction.h>
#include <matrix.h>

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

using TMatrix = Matrix<Fraction<TNumber>>;

const TMatrix kIdentity6 = {
    {1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1}
};

Fraction<TNumber> solveGoldMatrixes(const TLines &lines) {
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

/*
Or we'll put this in matrix form by Consider the v# are known, basically just guess and try them, some brute force... 
And write this system in matrix form. Since we know the times, only the first two sets of equations are needed:

p0.x = sx + t0 * (vx - d0.x)
p0.y = sy + t0 * (vy - d0.y)
p0.z = sz + t0 * (vz - d0.z)

p1.x = sx + t1 * (vx - d1.x)
p1.y = sy + t1 * (vy - d1.y)
p1.z = sz + t1 * (vz - d1.z)

p2.x = sx + t2 * (vx - d2.x)
p2.y = sy + t2 * (vy - d2.y)
p2.z = sz + t2 * (vz - d2.z)

and so on, leading to:

1       vx-d0.x     0       0           0       0                                       p0.x
1       vx-d1.x     0       0           0       0                       sx              p1.x
1       vx-d2.x     0       0           0       0                       t0              p2.x
0       0           1       vy-d0.y     0       0                       sy              p0.y
0       0           1       vy-d1.y     0       0           X           t1      ==      p1.y
0       0           1       vy-d2.y     0       0                       sz              p2.y
0       0           0       0           1       vz-d0.z                 t2              p0.z
0       0           0       0           1       vz-d1.z                                 p1.z
0       0           0       0           1       vz-d2.z                                 p2.z

aka M * ST = P
ST = (Mt*M)^(-1) * Mt * P, where Mt is M transposed 

Loop on some v# so that A in invertable and try.
Suppose v# are in similar to the hailstone v, so try in a similar range..
*/
    Line lineA = lines[3];
    Line lineB = lines[4];
    Line lineC = lines[5];

    TMatrix W = {
        {1, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 1, 0}
    };

    TMatrix P = {
        {static_cast<double>(lineA.p.x)},
        {static_cast<double>(lineB.p.x)},
        {static_cast<double>(lineC.p.x)},
        {static_cast<double>(lineA.p.y)},
        {static_cast<double>(lineB.p.y)},
        {static_cast<double>(lineC.p.y)},
        {static_cast<double>(lineA.p.z)},
        {static_cast<double>(lineB.p.z)},
        {static_cast<double>(lineC.p.z)}
    };

    const TNumber vMin = -300;
    const TNumber vMax = 300;

    for (TNumber vx = -110; vx<=-110; vx++) {
        auto m01 = vx - lineA.d.x;//vx - d0.x
        auto m11 = vx - lineB.d.x;
        auto m21 = vx - lineC.d.x;

        for (TNumber vy = -135; vy<=-135; vy++) {
            auto m33 = vy - lineA.d.y;
            auto m43 = vy - lineB.d.y;
            auto m53 = vy - lineC.d.y;

            for (TNumber vz = 299; vz<=299; vz++) {
                auto m65 = vz - lineA.d.z;
                auto m75 = vz - lineB.d.z;
                auto m85 = vz - lineC.d.z;

                TMatrix M = W;
                M[0][1] = m01;
                M[1][1] = m11;
                M[2][1] = m21;
                M[3][3] = m33;
                M[4][3] = m43;
                M[5][3] = m53;
                M[6][5] = m65;
                M[7][5] = m75;
                M[8][5] = m85;

                printMatrix(M, true, 3, "Matrix M:");
                TMatrix Mt = transposeMatrix(M);
                printMatrix(Mt, true, 3, "Transposed:");
                TMatrix MtM = multiplyMatrixes(Mt, M);
                printMatrix(MtM, true, 3, "Product:");
                TMatrix inverse;
                if (invertMatrix(MtM, inverse)) {
                    // ST = (Mt*M)^(-1) * Mt * P, where Mt is M transposed
                    printMatrix(inverse, true, 3, "MtM Inversed:");
                    TMatrix ST = multiplyMatrixes(inverse, Mt);
                    printMatrix(ST, true, 3, "ST on the way:");
                    ST = multiplyMatrixes(ST, P);
                    
                    printMatrix(ST, true, 3, "Solution: ");
                    
                    // check:
//                    if (ST[0][0] + ST[0][1]*vx == lineA.p.x + lineA.d.x * ST[0][1]) {
                        cout << "Matrix start x " << ST[0][0] << endl;
                        cout << "Matrix start y " << ST[2][0] << endl;
                        cout << "Matrix start z " << ST[4][0] << endl;
                        return (ST[0][0] + ST[2][0] + ST[4][0]);
//                    }
                        
                }
            }
        }
    }
    return 0;
}

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

    cout << "Start x " << solution.eval(sx) << endl;
    cout << "Start y " << solution.eval(sy) << endl;
    cout << "Start z " << solution.eval(sz) << endl;

    cout << "Speed x " << solution.eval(vx) << endl;
    cout << "Speed y " << solution.eval(vy) << endl;
    cout << "Speed z " << solution.eval(vz) << endl;

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
    
   cout << "GOLD Z3: ";
   const auto goldz3 = solveGoldEquation(geoLines);
   cout << "GOLD Z3: " << goldz3 << endl;
    
    cout << "GOLD Matrixes:" << endl;
    const auto goldm = solveGoldMatrixes(geoLines);
    cout << "GOLD Matrixes: " <<  goldm << endl;
    
    return EXIT_SUCCESS;
}
