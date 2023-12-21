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
    return (i << 32) + (j & 0xFFFFFFFF);
}

using TPostion = std::tuple<TNumber, TNumber>;

TPostion decodePosition(TNumber key) {
    const TNumber i = key >> 32;
    // cast or shifts:  j = ((key & 0xFFFFFFFF) << 32) >> 32;
    const TNumber j = static_cast<int32_t>(key & 0xFFFFFFFF);
    return {i, j};
}

using TReachedPositions = std::unordered_set<TNumber>;
using TMap = std::vector<std::string>;
using TStepMap = std::vector<std::vector<TNumber>>;

// Generalized to go outside the lines.
// For the silver solution it does not matter, becuase more than 65 steps are required to go outside the map
// And silver goes just 64 steps.
TReachedPositions goOneStep(const TMap &lines, TReachedPositions &positions) {

    TNumber nLines = static_cast<TNumber>(lines.size());
    TNumber nCols = static_cast<TNumber>(lines[0].size());// == nLines, particular  input
    const char kSTONE = '#';

    TReachedPositions nextPositions;
    const TNumber maxI = nLines - 1;
    const TNumber maxJ = nCols - 1;

    auto modulo = [](TNumber divident, TNumber divisor) {
        return ((divident % divisor) + divisor) % divisor;
    };
    
    auto makeStep = [&](TNumber i, TNumber j) {
        const auto mapI = modulo(i, nLines);
        const auto mapJ = modulo(j, nCols);
        if (kSTONE != lines[mapI][mapJ]) {
            nextPositions.insert(encodePosition(i, j));
        }
    };

    for (auto key : positions) {
        TNumber i, j;
        std::tie(i, j) = decodePosition(key);
        makeStep(i-1, j  );
        makeStep(i+1, j  );
        makeStep(i  , j-1);
        makeStep(i  , j+1);
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

    TNumber startI = 0;
    TNumber startJ = 0;

    TMap lines;
    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            const size_t pos = line.find('S');
            if (pos != std::string::npos) {
                startI = static_cast<TNumber>(lines.size());
                startJ = static_cast<TNumber>(pos);
                line[pos] = '.';
            }
            lines.push_back(std::move(line));
        }
    }

    // ________________________________________________________
    // SILVER
    // --------------------------------------------------------

    auto walk = [&](TNumber nbSteps) {
        TReachedPositions positions;
        positions.insert(encodePosition(startI, startJ));
        TReachedPositions nextPositions;
        for (size_t i = 0; i<nbSteps; i++) {
            nextPositions = goOneStep(lines, positions);
            positions.swap(nextPositions);
        }

        return positions.size();
    };

    const size_t silver = walk(64);
    cout << "Total silver is: " << silver << endl;

    // ________________________________________________________
    // GOLD
    // --------------------------------------------------------

    // Particular input: number of steps and stones' positions
    const size_t kNbGoldSteps = 26501365; // == 202300 * lines.size() + lines.size()/2 
    // The number of touched tiles is for the number of steps in the form above, aka below:
    // X = nSteps / lines.size, where nSteps = X * lines.size() + lines.size()/2 
    // Ntiles = A + b * X + c * X * X
    // The gold solution is found for  X == 202300
    // We need A, B and C

    TNumber X = 0;
    TNumber nSteps = lines.size()/2;// = 65
    const TNumber A = walk(nSteps);

    X = 1;
    nSteps = X * lines.size() + lines.size()/2;// = 196
    const TNumber A_B_C = walk(nSteps);

    X = 2;
    nSteps = X * lines.size() + lines.size()/2;// = 327
    const TNumber A_2B_4C = walk(nSteps);

    const TNumber B_C = A_B_C - A;
    assert((A_2B_4C - A) % 2 == 0);
    const TNumber B_2C  = (A_2B_4C - A) / 2;
    const TNumber C = B_2C - B_C;
    const TNumber B = B_C - C;

    nSteps = 26501365;
    X = nSteps / lines.size();
    TNumber gold = A + B * X + C * X * X;
    cout << "Total gold is: " << gold << endl;

    return EXIT_SUCCESS;
}
