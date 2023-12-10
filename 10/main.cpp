#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <vector>

using namespace std;

struct SPipeInfo;
using TMap = std::vector<std::vector<SPipeInfo>>;

bool isValidPosition(const TMap &fullMap, long i, long j) {
    return (i>=0) && (j>=0) && (i<fullMap.size()) && (j<fullMap.size());
}

bool isConnectedNorth(char myChar, char northChar) {
    switch(myChar) {
        case '|':
        case 'J':
        case 'L':
            return ((northChar == '|') || (northChar == '7') || (northChar == 'F'));
    }
    return  (myChar == 'S') &&
            ((northChar == '7') || (northChar == 'F') || (northChar == '|'));
}

bool isConnectedSouth(char myChar, char southChar) {
    switch(myChar) {
        case '|':
        case '7':
        case 'F':
            return ((southChar == '|') || (southChar == 'J') || (southChar == 'L'));
    }
    return (myChar == 'S') &&
           ((southChar == 'J') || (southChar == 'L') || (southChar == '|'));
}

bool isConnectedWest(char myChar, char westChar) {
    switch(myChar) {
        case '-':
        case 'J':
        case '7':
            return ((westChar == '-') || (westChar == 'L') || (westChar == 'F'));
    }
    return  (myChar == 'S') &&
            ((westChar == 'L') || (westChar == 'F') || (westChar == '-'));
}

bool isConnectedEast(char myChar, char eastChar) {
    switch(myChar) {
        case '-':
        case 'L':
        case 'F':
            return ((eastChar == '-') || (eastChar == 'J') || (eastChar == '7'));
    }
    return  (myChar == 'S') &&
            ((eastChar == '7') || (eastChar == 'J') || (eastChar == '-'));
}

struct SPipeInfo {
    char pipeShape;
    bool isVisited;
    int  distanceFromStart;

    SPipeInfo(char shape)
    : pipeShape(shape)
    , isVisited(false)
    , distanceFromStart(0)
    {};
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

    std::string line;
    TMap fullMap;
    size_t startI, startJ;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            const size_t startIdx = line.find('S');
            std::vector<SPipeInfo> mapLine;
            mapLine.reserve(line.size());
            for (char c:line) {
                mapLine.push_back(std::move(SPipeInfo(c)));
            }
            fullMap.push_back(std::move(mapLine));
            if (startIdx != std::string::npos) {
                startI = fullMap.size() - 1;
                startJ = startIdx;
            }
        }
    }

    auto goNorth = [&fullMap](size_t i, size_t j) {
        return (i>0)
                && !fullMap[i-1][j].isVisited
                && isConnectedNorth(fullMap[i][j].pipeShape, fullMap[i-1][j].pipeShape);
    };

    auto goSouth = [&fullMap](size_t i, size_t j) {
        return ((i + 1) <fullMap.size())
                && !fullMap[i+1][j].isVisited
                && isConnectedSouth(fullMap[i][j].pipeShape, fullMap[i+1][j].pipeShape);
    };

    auto goWest = [&fullMap](size_t i, size_t j) {
        return (j>0)
                && !fullMap[i][j-1].isVisited
                && isConnectedWest(fullMap[i][j].pipeShape, fullMap[i][j-1].pipeShape);
    };

    auto goEast = [&fullMap](size_t i, size_t j) {
        return ((j+1) < fullMap[0].size())
                && !fullMap[i][j+1].isVisited
                && isConnectedEast(fullMap[i][j].pipeShape, fullMap[i][j+1].pipeShape);
    };


    std::function<size_t(size_t, size_t, size_t)> navigate = [&](size_t i, size_t j, size_t distance) {
        // if ((fullMap[i][j] == 'S') && (distance  > 0)) {
        //     return distance;
        // }

        fullMap[i][j].isVisited = true;
        bool foundDir = false;
        if (goNorth(i,j)) {
            foundDir = true;
            return navigate(i-1, j, distance + 1);
        }
        if (goSouth(i,j)) {
            foundDir = true;
            return navigate(i+1, j, distance + 1);
        }
        if (goWest(i,j)) {
            foundDir = true;
            return navigate(i, j-1, distance + 1);
        }
        if (goEast(i,j)) {
            foundDir = true;
            return navigate(i, j+1, distance + 1);
        }
        return distance;
    };

    size_t distance;
    do {
        distance = navigate(startI, startJ, 0);
        if (distance > 0) {
            cout << "Distace total " << distance << endl;
            cout << "Distace max " << (distance + 1) / 2 << endl;
        }
    } while (distance > 0);

    return EXIT_SUCCESS;
}
