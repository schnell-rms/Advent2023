#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>
#include <utils.h>
#include <unordered_map>

using namespace std;

using TCache = std::unordered_map<std::string, size_t>;

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

    std::vector<string> lines;

    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            lines.push_back(std::move(line));
        }
    }

    long total = 0;
    auto kNbLines   = [&lines] () { return lines.size(); };
    auto kNbColumns = [&lines] () { return lines[0].size(); };
    std::vector<long> factors(kNbColumns(), kNbLines());
    // Get the first star in one shot: like all Os to the north and counted:
    for (size_t i=0; i<kNbLines(); ++i) {
        for (size_t j=0; j<kNbColumns(); ++j) {
            if (lines[i][j] == '#') {
                factors[j] = kNbLines() - i - 1;// - 1: calculate for the next line
            } else if (lines[i][j] == 'O') {
                total += factors[j];
                factors[j]--;
            }
        }
    }

    cout << "Total first star: " << total << endl;

    // Second star:
    auto getWeigth = [&]() {
        size_t northWeigth = 0;
        for (size_t i=0; i<kNbLines(); ++i) {
            for (size_t j=0; j<kNbColumns(); ++j) {
                if (lines[i][j] == 'O') {
                    northWeigth += kNbColumns() - i;
                }
            }
        }
        return northWeigth;
    };

    auto moveWest = [&]() {
        for (size_t i=0; i<kNbLines(); ++i) {
            size_t squarePos = -1;//overflow: ok!
            for (size_t j=0; j<kNbColumns(); ++j) {
                if (lines[i][j] == '#') {
                    squarePos = j;
                } else if (lines[i][j] == 'O') {
                    lines[i][j] = '.';
                    lines[i][squarePos + 1] = 'O';//maybe put 0 back...
                    squarePos++;
                }
            }
        }
    };

    auto clockwiseRotation = [&]() {
        std::vector<string> rotated(kNbColumns());
        for (size_t j=0; j<kNbColumns(); ++j) {
            rotated[j].reserve(kNbLines());
            for (size_t i=1; i<=kNbLines(); ++i) {
                rotated[j] += lines[kNbLines() - i][j];
            }
        }
        lines.swap(rotated);
    };

    // Face the north to the west:
    for (size_t i=0; i<3; i++) {
        clockwiseRotation();
    }

    TCache states;
    auto getCurrentKey = [&]() {
        std::string key;
        key.reserve(kNbColumns() * kNbLines());
        for (auto &line:lines) {
            key += line;
        }
        return key;
    };

    // unpacks the key. 
    // The 'lines' matrix will be in the key's rotation state to get the sizes right:
    auto unpackKey = [&](const std::string& key) {;
        std::vector<string> unpackedStates;
        unpackedStates.reserve(kNbLines());
        for (size_t i=0; i<kNbLines(); ++i) {
            unpackedStates.emplace_back(key.substr(i * kNbColumns(), kNbColumns()));
        }
        return unpackedStates;
    };

    const size_t kNbCycles = 1000000000UL;
    for (size_t i=0; i<kNbCycles; i++) {
        for (size_t j=0; j<4; j++) {
            moveWest();
            clockwiseRotation();
        }
        const std::string key = getCurrentKey();
        const auto it = states.find(key);
        if (states.end() != it) {
            // How many steps inside the last cycle to reach to state (kNbCycles - 1)? States start from 0:
            const size_t cycleLength = i - it->second;
            const size_t lastStateIdx = kNbCycles - 1;
            const size_t lastCycleStart = i + ((lastStateIdx - i) / cycleLength) * cycleLength;
            const size_t nbInsideCycle = lastStateIdx - lastCycleStart;
            const size_t solutionIdx = it->second + nbInsideCycle;
            const auto finalStateIt = std::find_if(states.cbegin(), states.cend(), [solutionIdx](auto it) {
                return (it.second == solutionIdx);
            });
            lines = unpackKey(finalStateIt->first);
            // Solution found, end loop:
            break;
        } else {
            states[key] = i;
        }
    }

    // Rotate North to north:
    clockwiseRotation();
    // printMatrix(lines);
    cout << "Total second star: " << getWeigth() << endl;

    return EXIT_SUCCESS;
}
