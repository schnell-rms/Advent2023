#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <utils.h>

using namespace std;

using TLense = std::pair<std::string, long>;

struct SBox {
    std::vector<TLense> lenses;
    void removeLense(const std::string& label) {
        for (size_t i=0; i<lenses.size(); ++i) {
            if (label == lenses[i].first) {
                lenses.erase(lenses.begin()+i);
                break;
            }
        }
    }
    
    void addLense(const std::string& label, long focalLength) {
        for (size_t i=0; i<lenses.size(); ++i) {
            if (label == lenses[i].first) {
                lenses[i].second = focalLength;
                return;
            }
        }
        // Lense not found:
        lenses.push_back({label, focalLength});
    }
    
    long power() {
        long pw = 0;
        for (size_t i=1; i<=lenses.size(); ++i)  {
            pw += i * lenses[i-1].second;;
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
