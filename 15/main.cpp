#include <iostream>
#include <sstream>
#include <fstream>

#include <string>

#include <utils.h>

using namespace std;

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
            code %= 256;
        }
        return code;
    };
        
    std::string line;
    getline(listFile, line);
    size_t totalSum = 0;
    if (!line.empty()) {
        size_t prevPos = 0;
        while (prevPos < line.size()) {
            size_t pos = line.find(",", prevPos);
            if (pos == std::string::npos) {
                pos = line.size();
            }
            const auto str = line.substr(prevPos, pos - prevPos);
            const auto code = hashCode(str);
            cout << str << ": " << code << endl;
            totalSum += code;
            prevPos = pos + 1;
        }
    }


    cout << "Total sum: " << totalSum << endl;

    return EXIT_SUCCESS;
}
