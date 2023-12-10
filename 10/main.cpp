#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <vector>

using namespace std;

struct SPipeInfo;
using TMap = std::vector<std::vector<SPipeInfo>>;

bool isConnectedNorth(char myChar, char northChar) {
    switch(myChar) {
        case '|':
        case 'J':
        case 'L':
            return ((northChar == '|') || (northChar == '7') || (northChar == 'F') || (northChar == 'S'));
    }
    return  (myChar == 'S') &&
            ((northChar == '7') || (northChar == 'F') || (northChar == '|'));
}

bool isConnectedSouth(char myChar, char southChar) {
    switch(myChar) {
        case '|':
        case '7':
        case 'F':
            return ((southChar == '|') || (southChar == 'J') || (southChar == 'L') || (southChar == 'S'));
    }
    return (myChar == 'S') &&
           ((southChar == 'J') || (southChar == 'L') || (southChar == '|'));
}

bool isConnectedWest(char myChar, char westChar) {
    switch(myChar) {
        case '-':
        case 'J':
        case '7':
            return ((westChar == '-') || (westChar == 'L') || (westChar == 'F') || (westChar == 'S'));
    }
    return  (myChar == 'S') &&
            ((westChar == 'L') || (westChar == 'F') || (westChar == '-'));
}

bool isConnectedEast(char myChar, char eastChar) {
    switch(myChar) {
        case '-':
        case 'L':
        case 'F':
            return ((eastChar == '-') || (eastChar == 'J') || (eastChar == '7') || (eastChar == 'S'));
    }
    return  (myChar == 'S') &&
            ((eastChar == '7') || (eastChar == 'J') || (eastChar == '-'));
}

enum EDirection {
    eUnknown = 0,
    eNorth,
    eSouth,
    eWest,
    eEast
};

struct SPipeInfo {
    char pipeShape;
    bool isVisited;
    int  distanceFromStart;
    EDirection toDirection;
    // Kind of redundant (to pipeShape and toDirection), but helpful:
    EDirection fromDirection;

    SPipeInfo(char shape)
    : pipeShape(shape)
    , isVisited(false)
    , distanceFromStart(0)
    , toDirection(eUnknown)
    , fromDirection(eUnknown)
    {};
    
    char getDirChar() const {
        if (!isVisited) {
            return pipeShape;
        }
        switch(toDirection) {
            case eNorth:
                return 'N';
            case eSouth:
                return 'S';
            case eWest:
                return 'W';
            case eEast:
                return 'E';
            default:
                break;
        }
        assert(false);
        return ' ';
    }

    bool isGoingNorth() const {
        return (eNorth == toDirection) || (eSouth == fromDirection);
    }

    bool isGoingSouth() const {
        return (eSouth == toDirection) || (eNorth == fromDirection);
    }

};

bool isClockWise(const SPipeInfo& leftVisited) {
    // The  way this is called, there is a guaranteed visited tile somewhere to the right of this one.
    // This leads to some constraints, for example this left visited tile could never be a '-'

    // So this left visited tile is always one of:
    //  | - north or south
    //  J - north or west
    //  7 - south or west
    // And overall the direction is north or south
    assert(leftVisited.pipeShape!='-');
    assert(leftVisited.pipeShape!='F');
    assert(leftVisited.pipeShape!='L');
    assert(leftVisited.toDirection != eEast);
    assert(leftVisited.isGoingNorth() || leftVisited.isGoingSouth());

    return leftVisited.isGoingNorth();
}

void setShapeOfStart(TMap& fullMap, size_t startI, size_t startJ) {
    SPipeInfo &startInfo = fullMap[startI][startJ];
    switch (startInfo.toDirection) {
        case eNorth: // J, L or |
            switch(startInfo.fromDirection) {
                case eNorth:
                    startInfo.pipeShape = '|';
                    break;
                case eWest:
                    startInfo.pipeShape = 'L';
                    break;
                default://case eEast:
                    startInfo.pipeShape = 'J';
                    break;
            }
            break;
        case eSouth: // 7 F or |
            switch(startInfo.fromDirection) {
                case eSouth:
                    startInfo.pipeShape = '|';
                    break;
                case eWest:
                    startInfo.pipeShape = 'F';
                    break;
                default://case eEast:
                    startInfo.pipeShape = '7';
                    break;
            }
            break;
        case eWest:// 7 J or -
            switch(startInfo.fromDirection) {
                case eWest:
                    startInfo.pipeShape = '-';
                    break;
                case eNorth:
                    startInfo.pipeShape = '7';
                    break;
                default://eSouth:
                    startInfo.pipeShape = 'J';
                    break;
            }
            break;
        case eEast: // L, F or -
            switch(startInfo.fromDirection) {
                case eEast:
                    startInfo.pipeShape = '-';
                    break;
                case eNorth:
                    startInfo.pipeShape = 'F';
                    break;
                default://case eSouth:
                    startInfo.pipeShape = 'L';
                    break;
            }
            break;
        default:
            assert(false);
    }
}

enum EPrint {
    eShape,
    eDirection
};

void printMap(const TMap& oneMap, EPrint what) {

    for(auto& line:oneMap) {
        std::vector<char> dirs(line.size(),' ');
        for(auto tile:line) {
            char c;
            if (what == eShape)
                c = tile.pipeShape;
            else
                c = tile.getDirChar();
            
            cout << ((c == 'A' || c == 'C') ? "\033[1;31m" : "\e[0m" ) << c << "  ";
        }
        cout << "\n";
        for(auto c:dirs) {
            cout << c << "  ";
        }
        cout << "\n";
    }
    cout << endl;
}

bool goNorth(const TMap& fullMap, size_t i, size_t j) {
    return (i>0)
            && !fullMap[i-1][j].isVisited
            && isConnectedNorth(fullMap[i][j].pipeShape, fullMap[i-1][j].pipeShape);
};

bool goSouth(const TMap& fullMap, size_t i, size_t j) {
    return ((i + 1) <fullMap.size())
            && !fullMap[i+1][j].isVisited
            && isConnectedSouth(fullMap[i][j].pipeShape, fullMap[i+1][j].pipeShape);
};

bool goWest(const TMap& fullMap, size_t i, size_t j) {
    return (j>0)
            && !fullMap[i][j-1].isVisited
            && isConnectedWest(fullMap[i][j].pipeShape, fullMap[i][j-1].pipeShape);
};

bool goEast(const TMap& fullMap, size_t i, size_t j) {
    return ((j+1) < fullMap[0].size())
            && !fullMap[i][j+1].isVisited
            && isConnectedEast(fullMap[i][j].pipeShape, fullMap[i][j+1].pipeShape);
};


size_t navigate(TMap& fullMap, size_t &i, size_t &j, size_t distance, EDirection from) {
    fullMap[i][j].isVisited = true;
    fullMap[i][j].fromDirection = from;
    if (goNorth(fullMap, i,j)) {
        fullMap[i][j].toDirection = eNorth;
        return navigate(fullMap, --i, j, distance + 1, eSouth);
    }
    if (goSouth(fullMap, i,j)) {
        fullMap[i][j].toDirection = eSouth;
        return navigate(fullMap, ++i, j, distance + 1, eNorth);
    }
    if (goWest(fullMap, i,j)) {
        fullMap[i][j].toDirection = eWest;
        return navigate(fullMap, i, --j, distance + 1, eEast);
    }
    if (goEast(fullMap, i,j)) {
        fullMap[i][j].toDirection = eEast;
        return navigate(fullMap, i, ++j, distance + 1, eWest);
    }
    
    cout << "Reached S " << i << " , " << j << endl;
    return distance;
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
            std::vector<SPipeInfo> mapLine;
            mapLine.reserve(line.size());
            for (char c:line) {
                mapLine.push_back(std::move(SPipeInfo(c)));
            }
            fullMap.push_back(std::move(mapLine));
            // Search for the start position:
            const size_t startIdx = line.find('S');
            if (startIdx != std::string::npos) {
                startI = fullMap.size() - 1;
                startJ = startIdx;
            }
        }
    }
    
    // I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
    // Get the path length, from S to just before S. Dummy first direction, will be overwritten.
    size_t lastI = startI, lastJ = startJ;
    size_t distance = navigate(fullMap, lastI, lastJ, 0, eNorth);
    // lastI, lastJ -> just before reaching start: could not go there because it was marked as visited:
    // Force one more step, reach S, to overwrite the S's direction towards it:
    fullMap[startI][startJ].isVisited = false;
    auto dummy = navigate(fullMap, lastI, lastJ, 0, fullMap[lastI][lastJ].fromDirection);


    cout << "Distace total " << distance << endl;
    cout << "Distace max " << (distance + 1) / 2 << endl;
    
    // II II II II II II II II II II II II II II II II II II II II II II II II
    bool isDebugMode = argc > 2;
    if (isDebugMode) {
        cout << "MAP:" << endl;
        printMap(fullMap, eShape);
    }
    
    // Count the tiles line by line, any tile between two visited tiles on that line.
    // Then inspect which one in on which side of the loop:
    //   - the clockwise side
    //   - the counter clockwise side
    // One of these two is the searched answer.

    // Steps:
    // 0. Set the shape of the start
    setShapeOfStart(fullMap, startI, startJ);
    cout << "Shape start is: " << fullMap[startI][startJ].pipeShape << endl;

    // Then:
    // 1. Traverse each line
    // 2. Get the tiles between visited tiles on that line
    // 3. For each such interval of tiles, determine the side: clockwise or not.
    // Count them accordingly.
    TMap debugMap;
    if (isDebugMode) {
        debugMap = fullMap;
    }
    
    EDirection leftSideDirection = eUnknown;
    size_t nbTilesInClockWise = 0;
    size_t nbTilesInCounterClockWise = 0;
    const size_t kNbCols = fullMap[0].size();
    for (size_t i=0; i<fullMap.size(); ++i) {
        std::vector<SPipeInfo> &fullLine = fullMap[i];
        size_t j = 0UL;
        // Find the left-most visited tile on this line.
        while ((j<kNbCols) && !fullLine[j].isVisited) {
            j++;
        }
        size_t startCheck = j;
        j++;

        if (eUnknown == leftSideDirection) {
            if (fullLine[startCheck].isGoingNorth()) {
                leftSideDirection = eNorth;
            } else if (fullLine[startCheck].isGoingSouth()) {
                leftSideDirection = eSouth;
            }
        } else {
            assert(eNorth == leftSideDirection  ? fullLine[startCheck].isGoingNorth()
                                                : fullLine[startCheck].isGoingSouth());
        }

        while (j < kNbCols) {
            // find the next visited tile on this line:
            while ((j<kNbCols) && !fullLine[j].isVisited) {
                j++;
            }

            if (j == kNbCols) {
                break;
            }

            // startCheck and j are both visited tiles:
            // In between there are tiles which need to be counted
            const size_t nbTiles = j - startCheck - 1;
            if (nbTiles > 0) {
                if (isClockWise(fullLine[startCheck])) {
                    nbTilesInClockWise += nbTiles;
                    if (isDebugMode) {
                        for (size_t col=startCheck+1; col<j; ++col) {
                            debugMap[i][col].pipeShape = 'C';
                        }
                    }
                } else {
                    nbTilesInCounterClockWise += nbTiles;
                    if (isDebugMode) {
                        for (size_t col=startCheck+1; col<j; ++col) {
                            debugMap[i][col].pipeShape = 'A';
                        }
                    }
                }
            }
            
            startCheck = j;
            j++;
        }
    }
    
    if (isDebugMode) {
        cout << "PROCESSED MAP:" << endl;
        printMap(debugMap, eShape);
        printMap(debugMap, eDirection);
    }

    cout << "Nb tiles:\n";
    cout << "Inside are the " << (leftSideDirection == eNorth ? "" : "counter ") << "clockwise tiles:\n";
    cout << " - clock wise: " << nbTilesInClockWise << "\n";
    cout << " - counter clock wise " << nbTilesInCounterClockWise << "\n";
    return EXIT_SUCCESS;
}
