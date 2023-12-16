#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>
#include <vector>

#include <utils.h>

using namespace std;

enum EDirection {
    eUp     = 0x01,
    eDown   = 0x02,
    eLeft   = 0x04,
    eRight  = 0x08
};

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
    
    std::vector<std::string> lines;

    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            lines.push_back(std::move(line));
        }
    }

    const size_t kSize = lines.size();
    assert(kSize == lines[0].size());
    std::vector<std::vector<size_t>> energy(kSize, std::vector<size_t>(kSize,0));
    std::vector<std::vector<size_t>> visitedFlags(kSize, std::vector<size_t>(kSize,0));

    auto outOfBounds = [kSize](ssize_t x) { return (x<0) || (x>=kSize); };

    std::function<void(ssize_t, ssize_t, EDirection)> reflect = [&](ssize_t i, ssize_t j, EDirection dir) {
        if (outOfBounds(i) || outOfBounds(j)) {
            return;
        }

        if (dir == (visitedFlags[i][j] & dir)) {
            return;
        }

        visitedFlags[i][j] |= dir;
        energy[i][j] += 1;

        auto goLeft = [&]() {
            reflect(i,j-1,eLeft);
        };

        auto goRight = [&]() {
            reflect(i,j+1,eRight);
        };

        auto goUp = [&]() {
            reflect(i-1,j,eUp);
        };

        auto goDown = [&]() {
            reflect(i+1,j,eDown);
        };

        switch (lines[i][j]) {
            case '.':
                switch(dir) {
                    case eUp:
                        return goUp();
                    case eDown:
                        return goDown();
                    case eLeft:
                        return goLeft();
                    case eRight:
                        return goRight();
                }
                assert(false);
            case '\\':
                switch(dir) {
                    case eUp:
                        return goLeft();
                    case eDown:
                        return goRight();
                    case eLeft:
                        return goUp();
                    case eRight:
                        return goDown();
                }
                assert(false);
            case '/':
                switch(dir) {
                    case eUp:
                        return goRight();
                    case eDown:
                        return goLeft();
                    case eLeft:
                        return goDown();
                    case eRight:
                        return goUp();
                }
                assert(false);
            case '|':
                switch(dir) {
                    case eUp:
                        return goUp();
                    case eDown:
                        return goDown();
                    case eLeft:
                    case eRight:
                        goUp();
                        return goDown();
                }
                assert(false);
            case '-':
                switch(dir) {
                    case eUp:
                    case eDown:
                        goLeft();
                        return goRight();
                    case eLeft:
                        return goLeft();
                    case eRight:
                        return goRight();
                }
                assert(false);
            default:
                assert(false);
        }
    };

    reflect(0,0, eRight);

    // printMatrix(energy);

    auto totalEnergy = [&]() {
        size_t totalSum = 0;
        for (size_t i=0; i<kSize; ++i) {
            for (size_t j=0; j<kSize; ++j) {
                totalSum += energy[i][j] > 0;
            }
        }
        return totalSum;
    };

    cout << "Total sum is: " << totalEnergy() << endl;

    auto clearAll = [&]() {
        for (size_t i = 0; i< kSize; ++i) {
            memset(energy[i].data(), 0, kSize*sizeof(size_t));
            memset(visitedFlags[i].data(), 0, kSize*sizeof(size_t));
        }
    };

    size_t maxEnergy = 0;
    for (size_t i = 0; i< kSize; ++i) {
        clearAll();
        reflect(i,0,eLeft);
        const auto sum = totalEnergy();
        if (sum > maxEnergy) {
            maxEnergy = sum;
        }
    }

    for (size_t i = 0; i< kSize; ++i) {
        clearAll();
        reflect(i,kSize-1,eRight);
        const auto sum = totalEnergy();
        if (sum > maxEnergy) {
            maxEnergy = sum;
        }
    }


    for (size_t j = 0; j< kSize; ++j) {
        clearAll();
        reflect(0,j,eDown);
        const auto sum = totalEnergy();
        if (sum > maxEnergy) {
            maxEnergy = sum;
        }
    }

    for (size_t j = 0; j< kSize; ++j) {
        clearAll();
        reflect(kSize-1,j,eUp);
        const auto sum = totalEnergy();
        if (sum > maxEnergy) {
            maxEnergy = sum;
        }
    }

    cout << "Total GOLD sum is: " << maxEnergy << endl;
    return EXIT_SUCCESS;
}
