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

std::pair<long, long> extrapolate(const std::string &line) {
    std::vector<long> numbers = allNumbers(line);

    std::vector<long> lastVals, firstVals;
    std::vector<long> nextNumbers;
    printVector(numbers);
    lastVals.push_back(numbers.back());
    firstVals.push_back(numbers[0]);
    while (!allZeroes(numbers)) {
        for (size_t i = 1; i < numbers.size(); ++i) {
            nextNumbers.push_back(numbers[i] - numbers[i-1]);
        }
        lastVals.push_back(nextNumbers.back());
        firstVals.push_back(nextNumbers[0]);

        numbers = std::move(nextNumbers);
        printVector(numbers);
    }

    cout << "LAST: ";
    printVector(lastVals);
    cout << "PAST: ";
    printVector(firstVals);
    long retFuture = 0, retPast = 0;

    
    for (ssize_t i = lastVals.size()-1; i > 0; --i) {
        retFuture += lastVals[i];
        retPast = firstVals[i-1] - retPast;
    }
    
    retFuture+= lastVals[0];
    
    // cout << ret << "\n\n" << endl;

    return std::make_pair(retFuture, retPast);
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
    long sumFuture = 0;
    long sumPast = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            auto ret = extrapolate(line);
            sumFuture += ret.first;
            sumPast  += ret.second;
        }
    }

    cout << "Sum: " << sumFuture << endl;
    cout << "Sum: " << sumPast << endl;
    return EXIT_SUCCESS;
}
