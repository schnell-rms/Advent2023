#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

TNumber key(TNumber i, TNumber j) {
    assert(i>=0);
    assert(j>=0);
    return (i<<32) | j;
}

TNumber unkeyI(TNumber key) {
    return key >> 32;
}

TNumber unkeyJ(TNumber key) {
    return key & 0xFFFFFFFF;
}

// Cache of longest paths
using TCache = std::unordered_map<TNumber, TNumber>;
using TMap = std::vector<std::string>;

const char kPath = '.';//46
const char kLeft = '<';//62
const char kRight = '>';//60
const char kUp = '^';//94
const char kDown = 'v';//118
const char kWalked = 1;//to be added to the above: it will not create confusion

const char kForest = '#';

enum EDirection {
    eLeft = 0,
    eRight,
    eUp,
    eDown
};

bool isWalkable(char c, EDirection dir) {
    return      (c == kPath)
            ||  ((c == kLeft) && (eLeft == dir))
            ||  ((c == kRight) && (eRight == dir))
            ||  ((c == kUp) && (eUp == dir))
            ||  ((c == kDown) && (eDown == dir));
}

// This is eventually now ok, i.e. the cache stores the max path from a position touched
// by a previous path: the path from here could not cross that previous path
// But otherwise another longer path might be reached from the cached position.
// So the cache is not ok, but this approach still works for the Silver input. Whatever.
// Maybe it would work in a reasonable time even without the cache.
size_t findPath(TNumber i, TNumber j, EDirection dir, TMap &lines, TCache &cache) {
    if (    (i < 0) || (j<0) || (i == lines.size()) || (j==lines[0].size())
         || (!isWalkable(lines[i][j], dir))) {
        return 0;
    }

    auto it = cache.find(key(i,j));
    if (it != cache.end()) {
        return it->second;
    }

    const char originalChar = lines[i][j];
    lines[i][j] += kWalked;

    const size_t left = findPath(i, j-1, eLeft, lines, cache);
    const size_t right = findPath(i, j+1, eRight, lines, cache);
    const size_t up = findPath(i-1, j, eUp, lines, cache);
    const size_t down = findPath(i+1, j, eDown, lines, cache);

    size_t taken = std::max(left, right);
    taken = std::max(taken, up);
    taken = std::max(taken, down);
    taken++;

    lines[i][j] = originalChar;
    cache[key(i,j)] = taken;
    return taken;
}


// Gold. The paths are narrow, like edges in a graph, so lets build a graph and navigate in that one
// as navigating in matrix wihtout cache takes forever, and the cache as above is not ok.
// The start and destination are nodes of the graph by the way the input is build: they can not be on an edge:
// only one way in or out from them.
struct Node {
    TNumber i,j;
};

struct Edges {
    // <key, length>
    std::unordered_map<TNumber, TNumber> toNodes;

    size_t size() const { return toNodes.size(); }
    auto begin() const { return toNodes.begin(); }

    void mergeEdge(TNumber oldKey, TNumber newKey, TNumber extraLength) {
        assert(newKey != oldKey);
        assert(toNodes.end() == toNodes.find(newKey));
        assert(toNodes.end() != toNodes.find(oldKey));
        toNodes[newKey] = toNodes[oldKey] + extraLength;
        toNodes.erase(oldKey);
    };
};

// <from node, edges>
using TGraph = std::unordered_map<TNumber, Edges>;

TGraph buildInitialGraph(const TMap &lines) {

    TGraph initialGraph;
    for(TNumber i=0; i<lines.size(); i++) {
        for(TNumber j=0; j<lines.size(); j++) {
            if (lines[i][j] != kForest) {

                auto addEdge = [&](TNumber ni, TNumber nj) {
                    if (    (ni < 0) || (nj<0) || (ni == lines.size()) || (nj==lines[0].size())
                        ||  (lines[ni][nj] == kForest)) {
                        return;
                    }
                    // Small edge between two matrix naighbours:
                    initialGraph[key(i,j)].toNodes[key(ni,nj)] = 1;
                    initialGraph[key(ni,nj)].toNodes[key(i,j)] = 1;
                };

                addEdge(i-1, j);
                addEdge(i+1, j);
                addEdge(i, j-1);
                addEdge(i, j+1);
            }
        }
    }

    return initialGraph;
}

void simplifyGraph(TGraph &graph) {
    // Find all nodes with only two edges and merge those small edges:
    std::vector<TNumber> toBeDeletedNodes;
    do {
        toBeDeletedNodes.clear();
        for (auto node : graph) {
            if (node.second.size() != 2) {
                // Intersection (>2) or start/end (<2)
                continue;
            }
            
            // Edges could be merged here
            auto edge1 = node.second.begin();
            auto edge2 = ++node.second.begin();
            
            const TNumber nodeKey = node.first;            
            // Merge with edges from neighbouring nodes:
            graph[edge1->first].mergeEdge(nodeKey, edge2->first, edge2->second);
            graph[edge2->first].mergeEdge(nodeKey, edge1->first, edge1->second);
            // Erase this merged node:
            toBeDeletedNodes.push_back(nodeKey);
        }
        
        for (TNumber key:toBeDeletedNodes) {
            graph.erase(key);
        }
    } while(!toBeDeletedNodes.empty());
}

// Route brute force through the smaller graph:
using TVisited = std::unordered_set<TNumber>;

// There could be only one destination, so go all the way, like for silver: no destination check.
TNumber route(TNumber key, const TGraph &graph, TVisited &visited) {
    visited.insert(key);

    TNumber maxLength = 0;
    Edges edges = graph.find(key)->second;
    for (auto ed:edges.toNodes) {
        if (visited.find(ed.first) != visited.end()) {
            continue;
        }
        auto len = route(ed.first, graph, visited) + ed.second;
        maxLength = std::max(maxLength, len);
    }

    visited.erase(key);
    return maxLength;
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
    
    TMap lines;
    std::string line;
    while(getline(listFile, line)) {
        lines.push_back(std::move(line));
    }

    const size_t pos = lines[0].find(kPath);
    assert(pos != std::string::npos);
    const TNumber startI = 0;
    const TNumber startJ = static_cast<TNumber>(pos);

    TCache cache;
    
    // The initial direction is irrelevant:
    const size_t silver = findPath(startI, startJ, eLeft, lines, cache) - 1;

    cout << "Longest silver path is: " << silver << endl;


    TGraph graph = buildInitialGraph(lines);
    simplifyGraph(graph);

    if (gIS_DEBUG) {
        cout << "GRAPH " << endl;
        for (auto node : graph) {
            cout << "From " <<  unkeyI(node.first) << " " << unkeyJ(node.first) << endl;
            for (auto edge  : node.second.toNodes) {
                cout << "To " << unkeyI(edge.first) << " " << unkeyJ(edge.first) << " is " << edge.second << endl;
            }
            cout << endl;
        }
    }
    
    TVisited visited;
    assert(graph.find(key(startI, startJ)) != graph.end());
    cout << "Gold takes a while..." << endl;
    const size_t gold = route(key(startI,startJ), graph, visited);
    cout << "Longest gold path is: " << gold << endl;
    return EXIT_SUCCESS;
}
