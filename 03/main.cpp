#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>

#include <vector>
#include <unordered_map>
#include <tuple>

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

using TGears = unordered_map<size_t, SGear>;

struct S3LinesGears {
    TGears previous;
    TGears current;
    TGears next;
    void goForward() {
        previous = std::move(current);
        current  = std::move(next);
    }
};

int realGearsSum(TGears gears) {
    int gearsSum = 0;
    for (auto it:gears) {
        if (it.second.isGear()) {
            gearsSum += it.second.ratio();
        }
    }
    return gearsSum;
}

const char kPointChar = '.';
const char kGearChar = '*';

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
                 (line[i] != kPointChar)) // and not a point
            {
                isConnected = true;
                if (line[i] == kGearChar) {
                    // a potential gear
                    gears[i].addNumber(number);
                }
            }
        }
        return isConnected;
    };

    auto getLineSum = [&](  const std::string& prevLine,
                            const std::string& currLine,
                            const std::string& nextLine,
                            size_t lineIndex,
                            S3LinesGears& gears)
    {
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
                // For the current line already look at, and only at, the neighbouring chars:
                if ((posStart > 0) and (currLine[posStart - 1] != kPointChar)) {
                    if (currLine[posStart - 1] == kGearChar) {
                        gears.current[posStart-1].addNumber(number);
                    }
                    isConnected = true;
                }

                if ((posEnd < currLine.size()) and (currLine[posEnd] != kPointChar)) {
                    if (currLine[posEnd] == kGearChar) {
                        gears.current[posEnd].addNumber(number);
                    }
                    isConnected = true;
                }

                // Have to run all hasSymbol, for the second star:
                isConnected = hasSymbol(prevLine, posStart, posEnd, lineIndex - 1, gears.previous, number) || isConnected;
                isConnected = hasSymbol(nextLine, posStart, posEnd, lineIndex + 1, gears.next, number) || isConnected;

                if (isConnected) {
                    lineSum += number;
                }
                
                searchStart = sm.suffix().first;
            }

            const int gearsSum = realGearsSum(gears.previous);
            gears.goForward();

            return std::make_tuple(lineSum, gearsSum);
    };
    
    int totalSum = 0;
    int totalGearSum = 0;
    std::string prevLine;
    std::string currLine;
    std::string nextLine;
    S3LinesGears gears;

    getline(listFile, currLine);
    size_t lineIndex = 0;
    while(getline(listFile, nextLine)) {
        if (!nextLine.empty()) {

            auto [partsSum, gearSum] = getLineSum(prevLine, currLine, nextLine,
                                                  lineIndex,
                                                  gears);
            totalSum += partsSum;
            totalGearSum += gearSum;
        }

        prevLine = std::move(currLine);
        currLine = std::move(nextLine);
        lineIndex++;
    }

    auto [partsSum, gearSum] = getLineSum(prevLine, currLine, nextLine, lineIndex, gears);
    totalSum += partsSum;
    totalGearSum += gearSum;

    cout << "Total sum: " << totalSum << endl;
    cout << "Gear sum: " << totalGearSum << endl;
    return EXIT_SUCCESS;
}
