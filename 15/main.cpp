#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <unordered_map>
#include <map>

#include <utils.h>

using namespace std;

using TLense = std::pair<std::string, long>;

struct SBox {
    //                <label <focalLength, orderNumber>>
    std::unordered_map<std::string, std::pair<long, size_t>> lenses;

    size_t lastIdx = 0;
    //  - vector: O(n) for both searching for the lense and then to remove it
    //  - map: O(logn) for removing
    void removeLense(const std::string& label) {
        auto it = lenses.find(label);
        if (it != lenses.end()) {
            const size_t sz = it->second.second;
            lenses.erase(it);
        }
    }
    
    // O(n) in vector version, because of searching for existance of the lense
    // O(logn) in map version
    void addLense(const std::string& label, long focalLength) {
        auto it = lenses.find(label);
        if (it != lenses.end()) {
            it -> second.first = focalLength;
        } else {// Lense not found:
            lenses[label] = {focalLength, ++lastIdx};
        }
    }
    
    // vector: O(n)
    // map: O(n * logn)
    long power() {
        // first order: O(n * log n)
        std::map<size_t, long> order;
        for (auto &it:lenses) {
            order[it.second.second] = it.second.first;
        }

        long pw = 0;
        size_t i = 0;
        // o(n)
        for (auto &it:order) {
            pw += (++i) * it.second;
        }

        return pw;
    }
};

int main(int argc, char *argv[]) {

    const bool isDebugMode = argc > 2;

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

    auto hashCode = [](const std::string &str) {
        size_t code = 0;
        for (char c:str) {
            code += static_cast<size_t>(c);
            code *= 17;
            code &= 0xFF;
        }
        return code;
    };
    
    std::vector<SBox> boxes(256);
    
    auto process = [&](const std::string &str) {
        const auto pos = str.find_first_of("=-");
        const auto label = str.substr(0,pos);
        const auto boxIdx = hashCode(label);
        if ('=' == str[pos]) {
            const long number = firstNumber(str);
            boxes[boxIdx].addLense(label, number);
        } else {
            boxes[boxIdx].removeLense(label);
        }
    };
    
    std::string line;
    getline(listFile, line);
    size_t totalSumFirstStar = 0;
    if (!line.empty()) {
        size_t prevPos = 0;
        while (prevPos < line.size()) {
            const size_t pos = std::min(line.find(",", prevPos), line.size());
            const auto str = line.substr(prevPos, pos - prevPos);
            // First star:
            const auto code = hashCode(str);
            totalSumFirstStar += code;
                
            // Second star:
            process(str);
            
            prevPos = pos + 1;
        }
    }

    long totalPower = 0;
    for (size_t i=1; i<=256; i++) {
        totalPower += i * boxes[i-1].power();;
    }
    
    cout << "Total sum first star: " << totalSumFirstStar << endl;
    cout << "Total sum second star: " << totalPower << endl;
    
    return EXIT_SUCCESS;
}
