#include <iostream>
#include <regex>
#include <string>
#include <vector>

bool gIS_DEBUG = false;

using TNumber = ssize_t;

std::vector<TNumber> allNumbers(const std::string& line) {
    std::smatch sm;
    std::vector<TNumber> ret;
    std::string::const_iterator searchStart(line.cbegin() );
    while (std::regex_search(searchStart, line.cend(), sm, std::regex("(-?\\d+)"))) {

        const TNumber number = std::stol(sm[1]);
        ret.push_back(number);

        searchStart = sm.suffix().first;
    }
    return ret;
};

TNumber firstNumber(const std::string& line) {
    std::smatch sm;
    std::regex_search(line.cbegin(), line.cend(), sm, std::regex("(-?\\d+)"));

    const TNumber number = std::stol(sm[1]);
    return number;
};

template<class T>
int printCollection(const T &whatever, bool endLine = true, int width = 3) {
    for(auto t:whatever) {
        std::cout << std::setw(width) << t << " ";
    }
    !whatever.empty() && std::cout << '\b';
    endLine && std::cout << std::endl;
    return EXIT_SUCCESS;
}

template<class T>
int printVector(const std::vector<T> &vec, bool endLine = true) {
    return printCollection(vec, endLine);
}

template<class T>
int printMatrix(const T &mat, bool endLine = true, int width = 3) {
    for (auto &vec:mat) {
        printCollection(vec, endLine, width);
    }
    std::cout << std::endl;
    return EXIT_SUCCESS;
}

int waitForKey() {
    return std::cin.get();
}
