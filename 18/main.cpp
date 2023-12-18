#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <utils.h>
#include <queue>
#include <unordered_map>

using namespace std;

using TInstructions = std::vector<std::string>; 
// using TDigPlan = std::unordered_map<size_t, std::pair<size_t, size_t>>; // [i][leftJ, rightJ]
using TDigPlan = std::vector<std::vector<size_t>>; // [i][leftJ, rightJ]

enum EDirection {
    eUp = 0,
    eDown,
    eLeft,
    eRight,
    eNoDirection
};

TDigPlan createPlan(const TInstructions& instructions) {
    ssize_t i = 0, j = 0;
    ssize_t ni = 0, nj = 0;
    ssize_t maxI = 0, maxJ = 0;
    ssize_t minI = 0, minJ = 0;
    for (const auto& line:instructions) {
        long steps = firstNumber(line);
        switch (line[0])
        {
        case 'R':
            j = j + steps;
            break;
        case 'L':
            j = j - steps;
            break;
        case 'U':
            i = i - steps;
            break;
        case 'D':
            i = i + steps;
            break;
        default:
            assert(false);
            break;
        }

        maxI = std::max(i, maxI);
        minI = std::min(i, minI);

        maxJ = std::max(j, maxJ);
        minJ = std::min(j, minJ);
        assert(maxI < 1000);
        assert(maxJ < 1000);
    }

    maxI += -minI;
    maxJ += -minJ;

    TDigPlan plan(maxI+1, std::vector<size_t>(maxJ+1, 0));

    i = -minI;
    j = -minJ;
    ni = i;
    nj = j;
    // Once more:
    for (const auto& line:instructions) {
        long steps = firstNumber(line);
        switch (line[0])
        {
        case 'R':
            nj = j + steps;
            break;
        case 'L':
            nj = j - steps;
            break;
        case 'U':
            ni = i - steps;
            break;
        case 'D':
            ni = i + steps;
            break;
        default:
            assert(false);
            break;
        }
        
        for (ssize_t mi=std::min(i,ni); mi <= std::max(i,ni); ++mi) {
            for (ssize_t mj=std::min(j,nj); mj <= std::max(j,nj); ++mj) {
                plan[mi][mj] = 1;
            }
        }

        i = ni;
        j = nj;
    }

    printMatrix(plan, true, 1);
    return plan;
}



bool fill(TDigPlan &plan, size_t key) {
    ssize_t startI = 0;
    ssize_t startJ = 0;
    bool found = false;
    for(size_t i = 0; (i < plan.size()) && !found; ++i) {
        for(size_t j = 0; j < plan[0].size(); ++j) {
            if (plan[i][j] == 0) {
                startI = i;
                startJ = j;
                found = true;
                break;
            }
        }
    }

    if (!found) {
        return false;
    }

    std::queue<std::pair<size_t, size_t>> q;
    q.push({startI, startJ});

    auto add = [&]( std::queue<std::pair<size_t, size_t>> &q,
                    ssize_t i, ssize_t j, ssize_t di, ssize_t dj) {
        i += di;
        j += dj;

        if ((i<0) ||  (j<0) || (i>=plan.size()) || (j>=plan[0].size())) {
            return;
        }

        if (plan[i][j] != 0) return;
        plan[i][j] = key;
        q.push({i,j});
    };

    plan[startI][startJ] = key;
    while (!q.empty()) {
        auto ij = q.front();
        q.pop();

        add(q, ij.first, ij.second, -1, 0);
        add(q, ij.first, ij.second, 1, 0);
        add(q, ij.first, ij.second, 0, 1);
        add(q, ij.first, ij.second, 0, -1);
    }

    printMatrix(plan);
    return true;
}

size_t area(TDigPlan &plan, size_t key) {
    size_t area = 0;
    // size_t i = 0;
    // TDigPlan::const_iterator it = plan.find(i);
    // while(it != plan.end()) {
    //     area += it->second.second - it->second.first + 1;
    //     it = plan.find(++i);
    // }

    // auto ok = [&](size_t j) { return j<plan[0].size();};

    for(size_t i = 0; i < plan.size(); ++i) {
        for(size_t j = 0; j < plan[0].size(); ++j) {
            area += (plan[i][j] == key);
        }
    }
    return area;
}

int main(int argc, char *argv[]) {

    gIS_DEBUG = argc > 2;

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
    
    std::vector<std::string> lines;
    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            lines.push_back(std::move(line));
        }
    }

    TDigPlan plan = createPlan(lines);
    size_t key = 2;
    while(fill(plan, key)) {
        key++;
    }

    //Silver: Choose biggest one + area(key == 1)
    for (size_t i=0; i<key; i++) {
        cout << "key " << i << " has area " << area(plan, i) << endl;
    }
    
    return EXIT_SUCCESS;
}
