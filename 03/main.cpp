#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>

#include <vector>
#include <unordered_map>

using namespace std;

struct SGear {
    void addNumber(int number)  {
        _numbers.push_back(number);
    }
    bool isGear() const { return _numbers.size() == 2; }
    int ratio() {
        int ret = 1;
        for (int n:_numbers) {
            ret *= n;
        }
        return ret;
    }
    private:
        std::vector<int> _numbers;
};

using TGears = unordered_map<uint64_t, SGear>;

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

    auto hasSymbol = [&](const std::string& line,
                         size_t startPos,
                         size_t endPos,
                         size_t lineIndex,
                         TGears& gears,
                         int number) {
        if (line.empty()) {
            return false;
        }
        
        bool isConnected = false;
        for (size_t i = ((startPos > 0) ? startPos - 1 : startPos);
             i < ((endPos < line.size()) ? endPos + 1 : line.size());
             ++i)
        {
            if (((line[i] < '0') || (line[i]>'9')) && // not a number
                 (line[i] != '.')) // and not a point
            {
                isConnected = true;
                if (line[i] == '*') // a potential gear
                {
                    gears[(lineIndex << 32) | i].addNumber(number);
                }
            }
        }
        return isConnected;
    };

    auto getLineSum = [&](  const std::string& prevLine,
                            const std::string& currLine,
                            const std::string& nextLine,
                            size_t lineIndex,
                            TGears& potentialGears)
    {
            const char kPointChar = '.';
            int lineSum = 0;
            // Get the numbers from the current line           
            smatch sm;
            string::const_iterator searchStart( currLine.cbegin() );
            while (regex_search(searchStart, currLine.cend(), sm, regex("(\\d+)"))) {
                // Validate the match:
                int number = std::stoi(sm[1]);
                auto posStart = (searchStart - currLine.begin()) + sm.position(1);
                auto posEnd = posStart + sm.str(1).size();

                bool isConnected = false;
                // For the second star, use hasSymbol() instead
                // if ((posStart > 0) and (currLine[posStart - 1] != kPointChar)) {
                //     isConnected = true;
                // }

                // if ((posEnd < currLine.size()) and (currLine[posEnd] != kPointChar)) {
                //     isConnected = true;
                // }

                // Have to run all hasSymbol, for the second star:
                isConnected = hasSymbol(prevLine, posStart, posEnd, lineIndex - 1, potentialGears, number) || isConnected;
                isConnected = hasSymbol(currLine, posStart, posEnd, lineIndex    , potentialGears, number) || isConnected;
                isConnected = hasSymbol(nextLine, posStart, posEnd, lineIndex + 1, potentialGears, number) || isConnected;

                // TODO: could optimize memory usage by storing every moment only the potential Gears from 2 lines

                if (isConnected) {
                    lineSum += number;
                }
                
                searchStart = sm.suffix().first;
            }

            return lineSum;
    };
    
    int totalSum = 0;
    std::string prevLine;
    std::string currLine;
    std::string nextLine;
    getline(listFile, currLine);
    size_t lineIndex = 0;
    TGears potentialGears;
    while(getline(listFile, nextLine)) {
        if (!nextLine.empty()) {

            totalSum += getLineSum(prevLine, currLine, nextLine, lineIndex, potentialGears);
        }

        prevLine = std::move(currLine);
        currLine = std::move(nextLine);
        lineIndex++;
    }

    totalSum += getLineSum(prevLine, currLine, nextLine, lineIndex, potentialGears);
    cout << "Total sum: " << totalSum << endl;

    // Filter gears:
    int gearsSum = 0;
    for (auto it:potentialGears) {
        if (it.second.isGear()) {
            gearsSum += it.second.ratio();
        }
    }

    cout << "Gear sum: " << gearsSum << endl;
    return EXIT_SUCCESS;
}
