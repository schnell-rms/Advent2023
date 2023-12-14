#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>
#include <utils.h>

using namespace std;

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

    std::vector<string> lines;

    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    long total = 0;
    const size_t kNbLines = lines.size();
    const size_t kNbColumns = lines[0].size();
    std::vector<long> factors(kNbColumns, kNbLines);
    printVector(lines);
    for (size_t i=0; i<kNbLines; ++i) {

        for (size_t j=0; j<kNbColumns; ++j) {
            if (lines[i][j] == '#') {
                factors[j] = kNbLines - i - 1;// - 1: calculate for the next line
            } else if (lines[i][j] == 'O') {
                total += factors[j];
                factors[j]--;
            }
        }
    }

    cout << "Total: " << total << endl;

    return EXIT_SUCCESS;
}
