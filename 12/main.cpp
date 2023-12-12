#include <iostream>
#include <sstream>
#include <fstream>

#include <optional>
#include <string>
#include <unordered_map>

#include <utils.h>

using namespace std;

using TMap = std::vector<std::string>;
using TArrangement = std::vector<std::vector<long>>;

using TCache = std::unordered_map<size_t, size_t>;

size_t hashSituation(size_t strIdx, size_t arrIdx, std::optional<size_t> mandatoryNb) {
    size_t ret = strIdx << 32;
    ret |= arrIdx << 16;
    ret |= (mandatoryNb.has_value() ? 1 : 0) << 15;
    ret |= (mandatoryNb.has_value() ? mandatoryNb.value() : 0);
    return ret;
}

size_t getNb(std::string str, size_t idx, std::vector<long> arr, size_t arrIdx,  std::optional<size_t> mandatoryNb, TCache& cache) {

    auto needsSpringNow = [&mandatoryNb]() {
        return mandatoryNb && (mandatoryNb.value() > 0);
    };

    auto needsPointNow = [&mandatoryNb]() {
        return mandatoryNb && (mandatoryNb.value() == 0);
    };

    if (str.size() == idx) {
        // All springs are counted. Need to count more?
        return (needsSpringNow() || (arrIdx < arr.size())) ? 0 : 1;
    }

    if ((arrIdx == arr.size()) && !needsSpringNow()) {
        // Nothing else to match, check whether there are still uncounted springs:
        return std::string::npos == str.find('#', idx) ? 1 : 0;
    }

    const size_t key = hashSituation(idx, arrIdx, mandatoryNb);
    // Inspect the cache:
    auto it = cache.find(key);
    if (it != cache.end()) {
        return it->second;
    }

    size_t nb;
    switch (str[idx]) {
        case '.':
            nb = needsSpringNow()
                    ? 0
                    : getNb(str, idx + 1, arr, arrIdx, std::optional<size_t>{}, cache);
            break;
        case '#':// Spring found:
            if (needsSpringNow()) {
                mandatoryNb = mandatoryNb.value() - 1;//count it
                nb = getNb(str, idx + 1, arr, arrIdx, mandatoryNb, cache);
            } else if (needsPointNow()) {
                nb = 0;
            } else {
                // Take this spring in the next group and retry:
                mandatoryNb = arr[arrIdx];// Start counting and run again:
                nb = getNb(str, idx, arr, arrIdx + 1, mandatoryNb, cache);
            }
            break;
        case '?':
            if (needsSpringNow()) { // Force it to spring:
                mandatoryNb = mandatoryNb.value() - 1;
                nb = getNb(str, idx + 1, arr, arrIdx, mandatoryNb, cache);
            } else if (needsPointNow()) { // FOrce it to point:
                nb = getNb(str, idx + 1, arr, arrIdx, std::optional<size_t>{}, cache);
            } else { // Try both:
                // Jump over as being a '.'
                const size_t nPoint = getNb(str, idx + 1, arr, arrIdx, std::optional<size_t>{}, cache);
                // Or retry, with mandatory # this time:
                mandatoryNb = arr[arrIdx];// start counter: next call will force it to spring:
                const size_t nSpring = getNb(str, idx, arr, arrIdx + 1, mandatoryNb, cache);
                nb = nPoint + nSpring;
            }
            break;
        default:
            assert(false);
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

    TMap springMap;
    TArrangement arrangement;

    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            size_t pos = line.find(' ');
            springMap.emplace_back(line.substr(0,pos));
            arrangement.push_back(allNumbers(line));
        }
    }

    auto unfold = [&]() {
        for (auto &str:springMap) {
            std::string unfoldedStr(str);
            for (int i=1; i<5;  i++) {
                unfoldedStr += '?';
                unfoldedStr += str;
            }
            str.swap(unfoldedStr);
        }
        for (auto &arr:arrangement) {
            auto newArr = arr;
            for (int i=0; i<4;  i++) {
                newArr.insert(std::end(newArr), std::begin(arr), std::end(arr));
            }
            arr.swap(newArr);
        }
    };

    size_t totalSum = 0;
    for (size_t i = 0; i< springMap.size(); ++i) {
        TCache cache;
        const auto lineNb = getNb(springMap[i], 0UL, arrangement[i], 0UL, std::optional<size_t>(), cache);
        totalSum += lineNb;
    }

    cout << "Silver sum: " << totalSum << endl;

    unfold();

    totalSum = 0;
    for (size_t i = 0; i< springMap.size(); ++i) {
        TCache cache;
        const auto lineNb = getNb(springMap[i], 0UL, arrangement[i], 0UL, std::optional<size_t>(), cache);
        totalSum += lineNb;
    }
    cout << "Golder sum: " << totalSum << endl;

    return EXIT_SUCCESS;
}
