#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>
#include <utils.h>
#include <vector>
#include <unordered_set>

using namespace std;

TNumber encodePosition(TNumber i, TNumber j) {
    return (i << 32) | j;
}

using TPostion = std::tuple<TNumber, TNumber>;

TPostion decodePosition(TNumber key) {
    return { key >> 32 , key & 0xFFFF};
}

using TReachedPositions = std::unordered_set<TNumber>;

const char kSTONE = '#';

TReachedPositions goOneStep(const std::vector<std::string> &lines, TReachedPositions &positions) {
    TReachedPositions nextPositions;
     const TNumber maxI = static_cast<TNumber>(lines.size()) - 1;
     const TNumber maxJ = static_cast<TNumber>(lines[0].size()) - 1;
     for (auto key : positions) {
         TNumber i, j;
         std::tie(i, j) = decodePosition(key);
         if ((i>0) && (kSTONE != lines[i-1][j])) {
            nextPositions.insert(encodePosition(i-1, j));
         }
         if ((i<maxI) && (kSTONE != lines[i+1][j])) {
            nextPositions.insert(encodePosition(i+1, j));
         }

         if ((j>0) && (kSTONE != lines[i][j-1])) {
            nextPositions.insert(encodePosition(i, j-1));
         }
         
         if ((j<maxJ) && (kSTONE != lines[i][j+1])) {
            nextPositions.insert(encodePosition(i, j+1));
         }
     }
    return nextPositions;
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
    
    TNumber startI = 0;
    TNumber startJ = 0;

    std::vector<std::string> lines;
    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            const size_t pos = line.find('S');
            if (pos != std::string::npos) {
                startI = static_cast<TNumber>(lines.size());
                startJ = static_cast<TNumber>(pos);
            }
            lines.push_back(std::move(line));
        }
    }

    const size_t nbSteps = 64;
    TReachedPositions positions;
    positions.insert(encodePosition(startI, startJ));
    TReachedPositions nextPositions;
    for (size_t i = 0; i<nbSteps; i++) {
        nextPositions = goOneStep(lines, positions);
        positions.swap(nextPositions);
    }

    cout << "Total silver is: " << positions.size() << endl;

    return EXIT_SUCCESS;
}
