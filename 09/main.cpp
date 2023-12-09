#include <iostream>
#include <sstream>
#include <fstream>

#include <algorithm>
#include <regex>
#include <string>
#include <vector>


using namespace std;

std::vector<long> allNumbers(const std::string& line) {
    smatch sm;
    std::vector<long> ret;
    string::const_iterator searchStart(line.cbegin() );
    while (regex_search(searchStart, line.cend(), sm, regex("(-?\\d+)"))) {

        const long number = std::stol(sm[1]);
        ret.push_back(number);

        searchStart = sm.suffix().first;
    }
    return ret;
};

bool allZeroes(std::vector<long> numbers) {
//    if (numbers.size() == 1) {
//        return true;
//    }
    for (long n: numbers) {
        if (n!=0) {
            return false;
        }
    }
    return true;
}

void printVector(const std::vector<long>& numbers) {
    for (long n:numbers) {
        cout << n << " ";
    }
    cout << endl;
}

long extrapolate(const std::string &line) {
    std::vector<long> numbers = allNumbers(line);

    std::vector<long> lastVals;
    std::vector<long> nextNumbers;
    printVector(numbers);
    lastVals.push_back(numbers.back());
    while (!allZeroes(numbers)) {
        for (size_t i = 1; i < numbers.size(); ++i) {
            nextNumbers.push_back(numbers[i] - numbers[i-1]);
        }
        lastVals.push_back(nextNumbers.back());

        numbers = std::move(nextNumbers);
        printVector(numbers);
    }

    cout << "LAST: ";
    printVector(lastVals);

    std::reverse(lastVals.begin(), lastVals.end());
    cout << "LAST: ";
    printVector(lastVals);

    long ret = 0;

    for (size_t i = 0; i < lastVals.size(); ++i) {
        ret += lastVals[i];
    }

    cout << ret << "\n\n" << endl;

    return ret;
};

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
    
    std::string line;
    long sumValue = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            sumValue  += extrapolate(line);
            
        }
    }

    cout << "Sum: " << sumValue << endl;
    return EXIT_SUCCESS;
}
