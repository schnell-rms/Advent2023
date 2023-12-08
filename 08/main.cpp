#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>

#include <unordered_map>

using namespace std;

long greatCommnDivisor(long x, long y)
{
    while(true)
    {
        if (x == 0) return y;
        y %= x;
        if (y == 0) return x;
        x %= y;
    }
}

long leastCommonMultiple(long x, long y)
{
    long gcd = greatCommnDivisor(x, y);

    return gcd ? (x / gcd * y) : 0;
}

using TMap = std::unordered_map<std::string, std::pair<std::string, std::string>>;

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

    TMap theMap;
    
    std::string line;
    getline(listFile, line);
    std::string directions = line;
    std::vector<std::string> positions;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            
            const std::string key = line.substr(0,3);
            theMap[key] = std::make_pair(line.substr(7,3), line.substr(12,3));
//            cout << key << " = " << theMap[key].first << " " << theMap[key].second << endl;
            if (key[2] == 'A') {
                positions.push_back(key);
            }
        }
    }

    auto isEnd = [&positions]() {
        for (auto& pos:positions) 
            if (pos.back() != 'Z') 
                return false;
        return true;
    };

    std::vector<std::string> starts;
    auto goOneStep = [&](char direction) {
        if (direction == 'L') {
            for (auto& pos : positions) {
                pos = theMap[pos].first;
            } 
        } else {
            for (auto& pos : positions) {
                pos = theMap[pos].second;
            } 
        }
    };


    auto findPath = [&](const std::string& start) {
        size_t i = 0;
        size_t nbSteps = 0;
        auto pos = start;
        while (pos.back() != 'Z') {
            pos = (directions[i] == 'L') ? theMap[pos].first : theMap[pos].second;
            i = (i+1) % directions.size();
            nbSteps++;
        };
        return nbSteps;
    };

    size_t nbSteps = findPath("AAA");
    cout << "NB steps: " << nbSteps << endl;

    size_t i = 0;
    std::vector<long> nbAllSteps(positions.size(), 0);
    for (size_t i=0; i<positions.size(); ++i) { 
        nbAllSteps[i] = findPath(positions[i]);
        cout << nbAllSteps[i] << endl;
    };

    nbSteps = nbAllSteps[0];
    for (i = 1; i< nbAllSteps.size(); ++i) {
        nbSteps = leastCommonMultiple(nbSteps, nbAllSteps[i]);
    }

    cout << "NB steps: " << nbSteps << endl;

    return EXIT_SUCCESS;
}
