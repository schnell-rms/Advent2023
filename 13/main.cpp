#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>

#include <utils.h>

using namespace std;

using TMap = std::vector<uint64_t>;

size_t checkReflection(TMap &numbers) {
    size_t ret = 0;
    const size_t sz = numbers.size();
    for (size_t i = 1; i<sz; ++i) {
        if (numbers[i] == numbers[i-1]) {
            const size_t reflectionSize = std::min(i,sz - i);
            bool isReflection = true;
            for (size_t j=0; j<reflectionSize; j++) {
                if (numbers[i+j] != numbers[i - j - 1]) {
                    isReflection = false;
                    break;
                }
            }
            if (isReflection) {
                return i;
            }
        }
    }
    return ret;
}

int main(int argc, char *argv[]) {

    const bool isDebug = argc > 2;

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

    auto binary = [&](const std::string& line, std::vector<uint64_t>& binLines, std::vector<uint64_t>& binCols) {
        uint64_t nb = 0;
        for (size_t i=0; i<line.length(); ++i) {
            const uint64_t bit = (line[i] == '#') ? 1 : 0;
            nb = (nb << 1) | bit;

            binCols[i] = (binCols[i] << 1) | bit;
        }
        binLines.push_back(nb);
    };

    std::vector<TMap> lineNumbers;
    std::vector<TMap> columnNumbers;

    size_t mapIdx = 0;

    std::string line;
    do {
        if (!line.empty()) {
            if (columnNumbers.back().empty()) {
                columnNumbers.back() = TMap(line.size(), 0UL);
            }
            binary(line, lineNumbers.back(), columnNumbers.back());
        } else {
            columnNumbers.push_back(TMap{});
            lineNumbers.push_back(TMap{});
        }
    } while(getline(listFile, line));

    printMatrix(lineNumbers);
    printMatrix(columnNumbers);

    size_t nbVerticalSymetries = 0;
    size_t nbHorizontalSymetries = 0;

    const size_t sz = lineNumbers.size();
    for (size_t i=0; i<sz; ++i) {
        nbVerticalSymetries += checkReflection(columnNumbers[i]);
        nbHorizontalSymetries += checkReflection(lineNumbers[i]);
    }

    const size_t result = nbVerticalSymetries + 100 * nbHorizontalSymetries;
    cout << "Result is: " << result << endl;

    return EXIT_SUCCESS;
}
