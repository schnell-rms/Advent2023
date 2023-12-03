#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>

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

    auto hasSymbol = [&](const std::string& line, size_t startPos, size_t endPos) {
        if (line.empty()) {
            return false;
        }
        
        for (size_t i = ((startPos > 0) ? startPos - 1 : startPos);
             i < ((endPos < line.size()) ? endPos + 1 : line.size());
             ++i)
        {
            if (((line[i] < '0') || (line[i]>'9')) && // not a number
                 (line[i] != '.')) // and not a point
            {
                return true;
            }
        }
        return false;
    };

    auto getLineSum = [&](const std::string& prevLine, const std::string& currLine, const std::string& nextLine) {
            const char kPointChar = '.';
            int lineSum = 0;
            // Get the numbers from the current line           
            smatch sm;
            string::const_iterator searchStart( currLine.cbegin() );
            while (regex_search(searchStart, currLine.cend(), sm, regex("(\\d+)"))) {
                // Validate the match:
                auto posStart = (searchStart - currLine.begin()) + sm.position(1);
                auto posEnd = posStart + sm.str(1).size();

                bool isConnected = false;
                if ((posStart > 0) and (currLine[posStart - 1] != kPointChar)) {
                    isConnected = true;
                }

                if ((posEnd < currLine.size()) and (currLine[posEnd] != kPointChar)) {
                    isConnected = true;
                }

                if (!isConnected) {
                    isConnected =   hasSymbol(prevLine, posStart, posEnd) ||
                                    hasSymbol(nextLine, posStart, posEnd);
                }

                if (isConnected) {
                    cout << sm[1] << endl;
                    lineSum += std::stoi(sm[1]);
                }
                
                searchStart = sm.suffix().first;
            }
            cout << "--------------" << endl;

            return lineSum;
    };
    
    int totalSum = 0;
    std::string prevLine;
    std::string currLine;
    std::string nextLine;
    getline(listFile, currLine);
    while(getline(listFile, nextLine)) {
        if (!nextLine.empty()) {

            totalSum += getLineSum(prevLine, currLine, nextLine);
        }

        prevLine = std::move(currLine);
        currLine = std::move(nextLine);
    }

    totalSum += getLineSum(prevLine, currLine, nextLine);
    cout << "Total sum: " << totalSum << endl;
    return EXIT_SUCCESS;
}
