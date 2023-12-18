#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <queue>
#include <limits>
#include <unordered_set>
#include <utils.h>

using namespace std;

enum EDirection {
    eUp = 0,
    eDown,
    eLeft,
    eRight,
    eNoDirection
};

using TCostMap = std::vector<std::string>;
using TVisited = std::unordered_set<size_t>;

const ssize_t kInitialCost = std::numeric_limits<ssize_t>::max();
ssize_t gLineSize = 0;
ssize_t gColSize = 0;
size_t gMaxDirCount = 0;
size_t gMinDirCount = 0;

struct SNavigationData {
    ssize_t i = 0;
    ssize_t j = 0;
    EDirection dir = eNoDirection;
    size_t dirCount = 0;
    size_t costToHere = kInitialCost;
    
    SNavigationData(ssize_t ii, ssize_t jj, EDirection dirr, size_t dirCountt, size_t costToHeree)
    : i(ii)
    , j(jj)
    , dir(dirr)
    , dirCount(dirCountt)
    , costToHere(costToHeree)
    {}
    
    SNavigationData(const SNavigationData &previousPosition,
                    EDirection nextDir,
                    const TCostMap &nodeCosts,
                    size_t comingCost)
    : SNavigationData(previousPosition)
    {
        dirCount = (dir == nextDir) ? dirCount + 1 : 1;
        dir = nextDir;

        // Gold star: jump over the first time, i.e. when direction was changed
        // aka when dirCount==1
        const ssize_t step = (dirCount == 1) ? gMinDirCount : 1;
        dirCount = (dirCount == 1) ? gMinDirCount : dirCount;

        costToHere = comingCost;
        switch (nextDir) {
            case eUp:
                for (ssize_t ii=std::max(0L,i-step); ii<i; ii++) {
                    costToHere += nodeCosts[ii][j] - '0';
                }
                i -= step;
                break;
            case eDown:
                for (ssize_t ii=i+1; ii<=std::min(i+step,gLineSize-1); ii++) {
                    costToHere += nodeCosts[ii][j] - '0';
                }
                i += step;
                break;
            case eRight:
                for (ssize_t jj=j+1; jj<=std::min(j+step,gColSize-1); jj++) {
                    costToHere += nodeCosts[i][jj] - '0';
                }
                j += step;
                break;
            case eLeft:
                for (ssize_t jj=std::max(0L,j-step); jj<j; jj++) {
                    costToHere += nodeCosts[i][jj] - '0';
                }
                j -= step;
                break;
            default:
                assert(false);
                break;
        }        
    }

    bool isValid() const {
        return (dirCount <= gMaxDirCount) && (i>=0) && (j>=0) && (i < gLineSize) && (j < gColSize);
    }

    bool operator<(SNavigationData const& other) const {
        return other.costToHere < costToHere;
    }

    size_t key() const {
        return (i << 20) | (j<<10) | (dirCount << 4) | (dir);
    }    
};

using TQueue = std::priority_queue<SNavigationData>;

void navigate(  TQueue &q, const TCostMap &nodeCosts, TVisited &isVisited,
                const SNavigationData &prevS, EDirection dir) {
    SNavigationData nextS(prevS, dir, nodeCosts, prevS.costToHere);
    if (!nextS.isValid()) {
        return;
    }

    if (isVisited.end() != isVisited.find(nextS.key())) {
        return;
    }
    
    isVisited.insert(nextS.key());
    q.push(nextS);
};

size_t dijkstra(const TCostMap &nodeCosts, size_t minDirCount, size_t maxDirCount) {
    // Set globals:
    gLineSize = nodeCosts.size();
    gColSize = nodeCosts[0].size();
    gMinDirCount = minDirCount;
    gMaxDirCount = maxDirCount;

    TQueue q;
    q.push({0,0,eNoDirection,0,0});

    TVisited isVisited;// Visited, aka already received its final cost, aka it was queued in the past
    isVisited.insert(q.top().key());

    auto destinationReached = [&](const SNavigationData &s) {
        return (s.i == nodeCosts.size() - 1) && (s.j == nodeCosts[0].size() - 1);
    };

    while (!q.empty() && !destinationReached(q.top()))
    {
        SNavigationData s = q.top();
        q.pop();

        //Do not turn, or turn only 90 degrees:
        if (eLeft   != s.dir)
            navigate(q, nodeCosts, isVisited, s, eRight);

        if (eRight  != s.dir)
            navigate(q, nodeCosts, isVisited, s, eLeft);

        if (eUp     != s.dir)
            navigate(q, nodeCosts, isVisited, s, eDown);
        
        if (eDown   != s.dir)
            navigate(q, nodeCosts, isVisited, s, eUp);
    }

    return q.top().costToHere;
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

    std::vector<std::string> lines;
    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            lines.push_back(std::move(line));
        }
    }

    // SILVER
    const size_t silver = dijkstra(lines, 1, 3);
    cout << "Total silver is: " << silver << endl;

    // GOLD STAR:
    const size_t gold = dijkstra(lines, 4, 10);
    cout << "Total gold is: " << gold << endl;

    return EXIT_SUCCESS;
}
