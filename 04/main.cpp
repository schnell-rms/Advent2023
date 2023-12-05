#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>

#include <unordered_set>
#include <unordered_map>

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

    auto cardWins = [&](const std::string& line) {
        int nbWins = 0;
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
                    nbWins++;
                }
            }

            searchStart = sm.suffix().first;
        }

        return nbWins;
    };

    int cardSum = 0;
    int nbCards = 0;
    std::unordered_map<int, int> factors;
    std::string line;
    int lineIndex = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            size_t pos = line.find(":");
            const int nbWins = cardWins(line.substr(pos + 1));

            factors[lineIndex]++;
            if (nbWins > 0) {
                for (int i=1; i<=nbWins; ++i) {
                    factors[lineIndex + i] += factors[lineIndex];
                }
                cardSum += 1 << (nbWins - 1);
            }

            nbCards += factors[lineIndex];
            factors.erase(lineIndex);

            lineIndex++;
        }
    }

    cout << "Card Sum: " << cardSum << endl;
    cout << "Nb Cards: " << nbCards << endl;
    return EXIT_SUCCESS;
}
