#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <set>
#include <string>

using namespace std;

#include <utils.h>


using TMap = std::vector<std::string>;

using TCoordinate = std::pair<ssize_t, ssize_t>;

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

    TMap originalMap;
    
    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            originalMap.push_back(std::move(line));
        }
    }

    const size_t kNbLines = originalMap.size();
    const size_t kNbCols = originalMap[0].size();

    std::vector<bool> isEmptyLine(kNbLines, true);
    std::vector<bool> isEmptyColumn(kNbCols, true);

    std::vector<TCoordinate> coordinates;
    
    for (size_t i=0; i<originalMap.size(); i++) {

        size_t pos = originalMap[i].find('#', 0);
        if (pos != std::string::npos) {
            isEmptyLine[i] = false;
            do {
                isEmptyColumn[pos] = false;
                coordinates.emplace_back(i,pos);
                pos = originalMap[i].find('#', pos + 1);
            } while(pos != std::string::npos);
        }
    }

    auto getDistance = [&](const TCoordinate &star1, const TCoordinate &star2) {

        // expansion factor: 2 for the first star. Value here is for the second star:
        const size_t expansionFactor = 1000000;

        const size_t distLine = abs(star2.first - star1.first);
        const size_t distCol = abs(star2.second - star1.second);
        size_t expansionLine = 0;
        const size_t minLine = std::min(star2.first, star1.first);
        const size_t maxLine = std::max(star2.first, star1.first);
        for (size_t i = minLine+1; i<maxLine; ++i) {
            if (isEmptyLine[i]) expansionLine++;
        }
        size_t expansionCol = 0;
        const size_t minCol = std::min(star2.second, star1.second);
        const size_t maxCol = std::max(star2.second, star1.second);
        for (size_t j = minCol+1; j<maxCol; ++j) {
            if (isEmptyColumn[j]) expansionCol++;
        }
        return distLine + distCol + (expansionFactor-1)*(expansionLine + expansionCol);
    };

    size_t totalSum = 0;
    for(size_t i=0; i<coordinates.size(); i++) {
        for(size_t j=i+1; j<coordinates.size(); j++) {
            const auto dist = getDistance(coordinates[i],coordinates[j]);
            isDebugMode && cout << "Dist " << i << " to " << j << " is: " << dist << "\n";
            totalSum += dist;
        }
    }

    cout << "Total sum: " << totalSum << endl;

    return EXIT_SUCCESS;
}
