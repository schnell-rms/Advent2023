#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>

#include <unordered_map>

using namespace std;

std::vector<long> allNumbers(const std::string& line) {
    smatch sm;
    std::vector<long> ret;
    string::const_iterator searchStart(line.cbegin() );
    while (regex_search(searchStart, line.cend(), sm, regex("(\\d+)"))) {

        const long number = std::stol(sm[1]);
        ret.push_back(number);

        searchStart = sm.suffix().first;
    }
    return ret;
};

long firstNumber(const std::string& line) {
    smatch sm;
    regex_search(line.cbegin(), line.cend(), sm, regex("(\\d+)"));

    const long number = std::stol(sm[1]);
    return number;
};

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
    while(getline(listFile, line)) {
        if (!line.empty()) {
            
            const std::string key = line.substr(0,3);
            theMap[key] = std::make_pair(line.substr(7,3), line.substr(12,3));
//            cout << key << " = " << theMap[key].first << " " << theMap[key].second << endl;
        }
    }

    std:string pos("AAA");
    size_t i = 0;
    size_t nbSteps = 0;
    while (pos != "ZZZ") {
        pos = (directions[i] == 'L') ? theMap[pos].first : theMap[pos].second;
        i = (i+1) % directions.size();
        nbSteps++;
    };


    cout << "NB steps: " << nbSteps << endl;

    return EXIT_SUCCESS;
}
