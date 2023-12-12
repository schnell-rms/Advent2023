#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <unordered_map>

#include <utils.h>

using namespace std;

using TCache = std::unordered_map<size_t, size_t>;

size_t hashSituation(size_t strIdx, size_t arrIdx) {
    return (strIdx << 32) | arrIdx;
}

size_t getNb(const std::string& str, size_t idx, const std::vector<long>& arr, size_t arrIdx, TCache& cache) {

    auto stillNeedsSprings = [&]() {
        return arrIdx < arr.size();
    };

    auto thereCouldBeSpringsLeft = [&]() {
        return std::string::npos != str.find_first_of("#?", idx);
    };

    auto thereAreSpringsLeft = [&]() {
        return std::string::npos != str.find('#', idx);
    };

    if (!thereCouldBeSpringsLeft()) { //former: (str.size() == idx) {
        // All springs are counted. Need to count more?
        return stillNeedsSprings() ? 0 : 1;
    }

    if (!stillNeedsSprings()) {
        // Nothing else to match, check whether there are still uncounted springs:
        return thereAreSpringsLeft() ? 0 : 1;
    }

    const size_t key = hashSituation(idx, arrIdx);
    // Inspect the cache:
    auto it = cache.find(key);
    if (it != cache.end()) {
        return it->second;
    }

    size_t nb = 0;
    // Hmmm .. all these could be fit in a nice & big tree of ternary operators... :))
    if ('.' == str[idx]) {
        if (stillNeedsSprings()) {
            if (thereCouldBeSpringsLeft()) {
                nb = getNb(str, idx + 1, arr, arrIdx, cache);
            }
        } else {// No need for further springs
            nb = thereAreSpringsLeft() ? 0 : 1;
        }
    }
    else if (('#' == str[idx]) || ('?' == str[idx])) {
        // Spring potetially found: forced to count it
        if (stillNeedsSprings()) {
            // Could fit here the entire arr[arrIdx] group?
            const size_t posPoint = std::min(   str.find('.', idx + 1),
                                                str.size());
            if (posPoint - idx >= arr[arrIdx]) {
                // The current group could expand up to:
                const size_t nextPos = idx + arr[arrIdx];
                // but no further:
                if (str[nextPos] != '#') {
                    // nbPos + 1 to jump over a '.' or a '?' which has to be point
                    nb = getNb(str, nextPos + 1, arr, arrIdx + 1, cache);
                }
            }
        }
    }

    if ('?' == str[idx]) {
        // Jump over as being a '.'
        // The '#' case was treated a
        nb += getNb(str, idx + 1, arr, arrIdx, cache);
    }
    
    cache[key] = nb;
    return nb;
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

    std::string springMap;
    std::vector<long> arrangement;

    auto unfold = [&]() {
        std::string unfoldedStr(springMap);
        for (int i=1; i<5;  i++) {
            unfoldedStr += '?';
            unfoldedStr += springMap;
        }
        springMap.swap(unfoldedStr);

        auto newArr(arrangement);
        for (int i=0; i<4;  i++) {
            newArr.insert(std::end(newArr), std::begin(arrangement), std::end(arrangement));
        }
        arrangement.swap(newArr);
    };

    std::string line;
    size_t totalSum_silver = 0;
    size_t totalSum_gold = 0;
    TCache cache;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            size_t pos = line.find(' ');
            springMap = line.substr(0,pos);
            arrangement = allNumbers(line);
            cache.clear();
            totalSum_silver += getNb(springMap, 0UL, arrangement, 0UL, cache);
            unfold();
            cache.clear();
            totalSum_gold   += getNb(springMap, 0UL, arrangement, 0UL, cache);
        }
    }

    cout << "Silver sum: " << totalSum_silver << endl;
    cout << "Golded sum: " << totalSum_gold << endl;

    return EXIT_SUCCESS;
}
