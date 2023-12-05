#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>
#include <vector>

using namespace std;

std::vector<long> getNumbers(const std::string& line) {
    smatch sm;
    std::vector<long> ret;
    string::const_iterator searchStart(line.cbegin() );
    while (regex_search(searchStart, line.cend(), sm, regex("(\\d+)"))) {
        // Validate the match:
        long number = std::stol(sm[1]);
        // auto posStart = (searchStart - line.begin()) + sm.position(1);
        
        ret.push_back(number);

        searchStart = sm.suffix().first;
    }
    return ret;
};


struct SMap {
    SMap(long dest, long start, long len)
    :_dest(dest)
    ,_source(start)
    ,_length(len)
    {}
    long _dest;
    long _source;
    long _length;

    long convert(long input) {
        if ((input >= _source) && (input < _source + _length))
        {
            return input - _source + _dest;
        }
        return -1;
    }
};

SMap readMap(const std::string& line) {
    std::vector<long> vals = getNumbers(line);
    return SMap(vals[0], vals[1], vals[2]);
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

    // get seeds
    std::string line;
    getline(listFile, line);
    std::vector<long> seeds = getNumbers(line);

    // GET ALL THE MAPS:
    std::vector<std::vector<SMap>> maps(7);
    long mapIdx = -1;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                mapIdx++;
                continue;
            }

            maps[mapIdx].emplace_back(readMap(line));
        }
    }

    auto getLocation = [&](long seed) {
        for (std::vector<SMap>& ms:maps) {
            long dest = -1;
            size_t idx = 0;
            while ((dest == -1) && (idx < ms.size())) {
                dest = ms[idx].convert(seed);
                idx++;
            }
            
            if (dest != -1) {
                seed = dest;
            }
//            else {
//same value
//            }
        }

        return seed;
    };

    // GET THE LOCATIONS:
    long minLocation = std::numeric_limits<long>::max();
    for (long oneSeed : seeds) {
        const long location = getLocation(oneSeed);
        if (location < minLocation) {
            minLocation = location;
        }
    }

    cout << "Min Location: " << minLocation << endl;
    return EXIT_SUCCESS;
}
