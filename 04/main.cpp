#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <optional>
#include <string>

#include <unordered_set>

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

    auto cardValue = [&](const std::string& line) {
        std::optional<int> score;
        size_t delimiterPos = line.find("|");
        smatch sm;
        string::const_iterator searchStart(line.cbegin() );
        std::unordered_set<int> win, got;
        while (regex_search(searchStart, line.cend(), sm, regex("(\\d+)"))) {
            // Validate the match:
            int number = std::stoi(sm[1]);
            auto posStart = (searchStart - line.begin()) + sm.position(1);
            if (posStart < delimiterPos) {
                win.insert(number);
            } else {
                if (win.find(number) != win.end()) {
                    score = score ? score.value() * 2 : 1;
                }
            }

            searchStart = sm.suffix().first;
        }

        return score ? score.value() : 0;
    };

    int cardSum = 0;
    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            size_t pos = line.find(":");
            cardSum += cardValue(line.substr(pos + 1));
        }
    }

    cout << "Card Sum: " << cardSum << endl;
    return EXIT_SUCCESS;
}
