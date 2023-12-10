#include <iostream>
#include <sstream>
#include <fstream>

#include <numeric>
#include <regex>
#include <string>
#include <unordered_map>

using namespace std;

using TMap = std::unordered_map<std::string, std::pair<std::string, std::string>>;

template<class T>
void printVector(const std::vector<T> vec) {
    for(auto t:vec) {
        cout << t << " ";
    }
    cout << endl;
}

int main(int argc, char *argv[]) {

    const bool isDebugMode = argc > 2;

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

    TMap theMap;

    std::string line;
    getline(listFile, line);
    std::string directions = line;
    std::vector<std::string> positions;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            const std::string key = line.substr(0,3);
            theMap[key] = std::make_pair(line.substr(7,3), line.substr(12,3));
            if (key[2] == 'A') {
                positions.push_back(key);
            }
        }
    }

    auto goOneStep = [&](char direction) {
        if (direction == 'L') {
            for (auto& pos : positions) {
                pos = theMap[pos].first;
            } 
        } else {
            for (auto& pos : positions) {
                pos = theMap[pos].second;
            } 
        }
    };

    // It seems that the input in engineered in such a way that after reaching one Z
    // the same Z will be reached again and again in a cycle, wihtout passsing through
    // another Z.
    // findPath returns the number of steps until the first and second Z
    auto findPath = [&](const std::string& start) {
        std::vector<long> nbStepsToZ;
        if (theMap.end() == theMap.find(start)) {
            return nbStepsToZ;
        }
        size_t i = 0;
        auto pos = start;
        long times = 0;
        const long kNbNbOfSearchedZs = isDebugMode ? 10 : 2;// first Z=>Z cycle only
        nbStepsToZ.push_back(0);
        while (times < kNbNbOfSearchedZs) {
            assert(theMap.end() != theMap.find(pos));
            while (pos.back() != 'Z') {
                // isDebugMode && cout << pos << " ";
                pos = (directions[i] == 'L') ? theMap[pos].first : theMap[pos].second;
                i = (i+1) % directions.size();
                nbStepsToZ.back()++;
            };
            // isDebugMode && cout << pos << " ";
            pos = (directions[i] == 'L') ? theMap[pos].first : theMap[pos].second;
            i = (i+1) % directions.size();
            nbStepsToZ.push_back(1);
            times++;
        }

        // isDebugMode && cout << endl;
        return nbStepsToZ;
    };

    // I I I I I I I
    // First star:
    auto result = findPath("AAA");
    size_t nbSteps = 0;
    if (!result.empty()) {
        nbSteps = result[0];
        cout << "NB steps: " << nbSteps << endl;
    }

    // II II II II II II II II II II
    // The Z=>Z cycles have the same number of steps as the A=>Z path
    // So use the least common multiple
    size_t i = 0;
    size_t kNbRoutes = positions.size();
    // Route info: offset, steps to first Z, ....
    std::vector<std::vector<long>> routeInfo;
    routeInfo.reserve(kNbRoutes);
    for (size_t i=0; i<positions.size(); ++i) {
        routeInfo.push_back(findPath(positions[i]));
    };

    nbSteps = routeInfo[0][0];
    for (i = 1; i< routeInfo.size(); ++i) {
        nbSteps = std::lcm(nbSteps, routeInfo[i][0]);
    }

    cout << "NB steps: " << nbSteps << endl;

    // III III III III III III III III III III III
    // More general approach, with different offsets - aka A=>Z steps - of the Z-cycles,
    // although not he case in the given input.
    // Interesting whether such a case could be easily constructed, i.e. A==>Z number of steps
    // different than the Z==> same and only Z cycle.
    // Becausue the LR directions from A to entering the cycle would have to become part of the cycle,
    // in the next runs and so on so forth.
    // Anyway, for the sake of it: similar with the least common multiplier with different starting offsets.
    // When offsets are 0, it is real lcm:
    if (isDebugMode && ("offset" == std::string(argv[2]))) {
        cout << "Start the different offsets computation" << endl;
        std::vector<long> routeSteps(kNbRoutes);

        for (size_t i=0; i<kNbRoutes; ++i) {
            routeSteps[i] = routeInfo[i][0];
        };

        auto allOnZ = [&]() {
            for (size_t i=1; i<kNbRoutes; ++i) {
                if (routeSteps[i-1] != routeSteps[i])
                    return false;
            }
            return routeSteps[0]!=0;
        };

        clock_t tStart = clock();
        long counter = 0;
        while (!allOnZ()) {
            long minSteps = routeSteps[0];
            for (size_t i=1; i<kNbRoutes; ++i) {
                if (minSteps > routeSteps[i]) {
                    minSteps = routeSteps[i];
                }
            }

            for (size_t i=0; i<kNbRoutes; ++i) {
                if (minSteps == routeSteps[i]) {
                    routeSteps[i] += routeInfo[i][1];
                }
            }

            counter++;
        }

        cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;

        cout << "Counter on " << counter << endl;
        cout << "NB steps - different offsets considered: " << routeSteps[0] << endl;
        assert(nbSteps == routeSteps[0]);
    }

    // IV IV IV IV IV IV IV IV IV IV IV IV IV IV IV IV IV IV IV IV IV
    if (isDebugMode) {
        auto ttt = findPath("AAA");
        cout << "cycles AAA" << endl;
        printVector<long>(ttt);

        ttt = findPath("A1A");
        cout << "cycles A1A" << endl;
        printVector<long>(ttt);
    }

    return EXIT_SUCCESS;
}
