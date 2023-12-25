#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>

#include <unordered_set>
#include <iterator>

using namespace std;

struct Node;

// <node names, Node>
using TGraph = std::unordered_map<std::string, Node>;

class Node {

public:
    void addEdge(const std::string &node, size_t connections) {
        nbours[node] = connections;
    }

    void removeEdge(const std::string &node) {
        nbours.erase(node);
    }

    void mergeWithRandomNeighbour(TGraph &g) {
        assert(!nbours.empty());
        const auto nodeIdx = rand() % nbours.size();
        auto it = nbours.begin();
        std::advance(it, nodeIdx);
        const std::string nameOfMergedNode = it->first;
        const size_t nConnections = it->second;
        nbours.erase(it);
        numberOfMerged += g[nameOfMergedNode].nbMerged();

        // gIS_DEBUG && cout << " absorb " << nameOfMergedNode << " into " << myName << endl;

        g[nameOfMergedNode].replaceMeBy(myName, g, nConnections);


        for (auto &s:g[nameOfMergedNode].absorbed) {
            assert(absorbed.find(s) == absorbed.end());
            absorbed.insert(s);
        }

        g.erase(nameOfMergedNode);
    }

    size_t nbMerged() const { return numberOfMerged; }

    size_t nbFinalEdges() const { return nbours.begin()->second; }
    
    void setName(const std::string &n) { myName = n; absorbed.insert(n); }
    
private:
    void replaceMeBy(const std::string &absorber, TGraph &g, size_t nConnFromReplacer) {
        assert(nbours.find(absorber) != nbours.end());
        const size_t nConnectionsToReplacer = nbours.find(absorber)->second;
        assert(nConnectionsToReplacer == nConnFromReplacer);
        nbours.erase(absorber);
        for (auto s:nbours) {
            const size_t nConn = g[s.first].nbours[myName] + g[s.first].nbours[absorber];
            g[s.first].removeEdge(myName);
            g[s.first].addEdge(absorber, nConn);
            g[absorber].addEdge(s.first, nConn);
        }
    }
    
public:
    std::string myName;
    // <neightbour name, connections to it>
    std::unordered_map<std::string, size_t> nbours;//neighbours
    size_t numberOfMerged = 1;//start with itself
    size_t numberOfEdges = 0;
    std::unordered_set<std::string> absorbed;
};

TNumber kargetMinCut(const TGraph &graph) {

    TGraph g;
    size_t n1, n2;
    size_t nTry = 0;
    size_t nCuts;
    do {
        g = graph;

        while (g.size() > 2) {
            // Select an edge, by first selecting a node:
            const auto nodeIdx = rand() % g.size();
            auto nodeIt = g.begin();
            std::advance(nodeIt, nodeIdx);
            nodeIt->second.mergeWithRandomNeighbour(g);
        }

        auto it1 = g.begin();
        auto it2 = ++g.begin();
        
        n1 = it1->second.nbMerged();
        n2 = it2->second.nbMerged();
        
        assert(it1->second.nbFinalEdges() == it2->second.nbFinalEdges());
        gIS_DEBUG && cout << "Final nodes: " << it1->first << " containing:" << endl;
        gIS_DEBUG && printCollection(it1->second.absorbed);
        gIS_DEBUG && cout << "And " << it2->first << " containing: " << endl;
        gIS_DEBUG && printCollection(it2->second.absorbed);
        gIS_DEBUG && cout << "Split it two: " << n1 << " and " << n2 << " by cutting " << it1->second.nbFinalEdges() << " edges" << endl;

        assert(graph.size() == (n1+n2));
        nCuts = g.begin()->second.nbFinalEdges();
        nTry++;
    } while ((nTry < 10000) && (nCuts != 3));

    cout << "Nb tries " << nTry << endl;
    (nCuts != 3) && cout << "Might need to try again..." << endl;

    return n1 * n2;
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

    TGraph graph;

    std::string line;
    std::vector<std::string> nodeNames;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            nodeNames = allAlphaStrings(line);
            for (size_t i=1; i<nodeNames.size(); ++i) {
                graph[nodeNames[0]].addEdge(nodeNames[i],1UL);
                graph[nodeNames[i]].addEdge(nodeNames[0],1UL);
            }
        }
    }
    
    srand(static_cast<unsigned int>(time(NULL)));

    // Not necessary, could use and pass the TGraph key everywhere, but whatever:
    for (auto &it:graph) {
        it.second.setName(it.first);
    }

    cout << "Apply Karger's algorithm in a loop until 3-edges cut split is found. Could be optimized to Karger–Stein algorithm. Perhaps another time..."  << endl;
    const auto silver = kargetMinCut(graph);
    cout << "Silver == Gold: " << silver << endl;

    return EXIT_SUCCESS;
}
