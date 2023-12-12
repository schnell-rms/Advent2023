#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>

#include <utils.h>

using namespace std;

using TMap = std::vector<std::string>;
using TArrangement = std::vector<std::vector<long>>;

bool isMatch(const std::string& str, const std::vector<long> &arr) {
        assert(std::string::npos == str.find('?'));
        size_t i=0;
        size_t ssz = str.size();
        size_t asz = arr.size();
        size_t j = 0;
        while ((i<ssz) && (j<asz)) {
            while ((i<ssz) && (str[i]=='.')) ++i;
            long count = 0;
            while ((i<ssz) && (str[i]=='#')) {
                count++;
                ++i;
            }
            if (count != arr[j]) {
                return false;
            }

            j++;
        }
        if (j!=asz) return false;
        if (std::string::npos == str.find('#',i)) {
            return true;
        }
        return false;
    };

size_t getNb(std::string str, const std::vector<long> &arr) {
    size_t pos = str.find('?');
    size_t n=0;
    if (pos != std::string::npos) {
        str[pos] = '.';
        n = getNb(str, arr);
        str[pos] = '#';
        n += getNb(str, arr);
    } else if (isMatch(str, arr)) {
        return 1UL;
    }
    
    return n;
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
    
    
    auto is = isMatch("..#..#....###.", {1,1,3});
    cout << (is ? "DA" : "NU") << endl;
    
    auto n = getNb(".??..??...?##.", {1,1,3});
    cout << "AAAA " << n << endl;
    
    TMap springMap;
    TArrangement arrangement;

    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            size_t pos = line.find(' ');
            springMap.emplace_back(line.substr(0,pos));
            arrangement.push_back(allNumbers(line));
            // cout << springMap.back() << endl;
            // printVector(arrangement.back());
        }
    }

    size_t totalSum = 0;
    for (size_t i = 0; i< springMap.size(); ++i) {
        auto lineNb = getNb(springMap[i], arrangement[i]);
        totalSum += lineNb;
    }

    cout << "Total sum: " << totalSum;

    return EXIT_SUCCESS;
}
