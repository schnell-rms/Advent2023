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
            for (size_t j=1; j<reflectionSize; j++) {
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

bool isPowerOf2(uint64_t x) {
    // aka has at most one 1 bit
    return (x & (x-1)) == 0;
}

bool equalSmudgeNumbers(uint64_t x, uint64_t y, size_t &smudgeCount) {
    if (x == y) 
        return true;
    const uint64_t eval = x^y;
    const bool itIs = isPowerOf2(eval);
    if (itIs) {
        smudgeCount++;
    }
    return itIs;
}

size_t checkReflectionSmudge(TMap &numbers) {
    size_t ret = 0;
    const size_t sz = numbers.size();
    for (size_t i = 1; i<sz; ++i) {
        size_t smudgeCount = 0;
        if (equalSmudgeNumbers(numbers[i], numbers[i-1], smudgeCount)) {
            const size_t reflectionSize = std::min(i,sz - i);
            bool isReflection = true;
            for (size_t j=1; j<reflectionSize; j++) {
                if (    !equalSmudgeNumbers(numbers[i+j],numbers[i - j - 1], smudgeCount) 
                    ||  (smudgeCount > 1)) {
                    isReflection = false;
                    break;
                }
            }
            if (isReflection && (smudgeCount == 1)) {
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

    if (isDebug) {
        printMatrix(lineNumbers);
        printMatrix(columnNumbers);
    }

    size_t nbVerticalSymetries = 0;
    size_t nbHorizontalSymetries = 0;

    size_t nbVerticalSymetries_smudge = 0;
    size_t nbHorizontalSymetries_smudge = 0;

    const size_t sz = lineNumbers.size();
    for (size_t i=0; i<sz; ++i) {
        nbVerticalSymetries += checkReflection(columnNumbers[i]);
        nbHorizontalSymetries += checkReflection(lineNumbers[i]);

        nbVerticalSymetries_smudge += checkReflectionSmudge(columnNumbers[i]);
        nbHorizontalSymetries_smudge += checkReflectionSmudge(lineNumbers[i]);
    }

    const size_t result = nbVerticalSymetries + 100 * nbHorizontalSymetries;
    cout << "Result is: " << result << endl;

    const size_t result_smudge = nbVerticalSymetries_smudge + 100 * nbHorizontalSymetries_smudge;
    cout << "Smudge result is: " << result_smudge << endl;
    return EXIT_SUCCESS;
}
