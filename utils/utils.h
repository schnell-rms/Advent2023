#include <iostream>
#include <regex>
#include <string>
#include <vector>

std::vector<long> allNumbers(const std::string& line) {
    std::smatch sm;
    std::vector<long> ret;
    std::string::const_iterator searchStart(line.cbegin() );
    while (std::regex_search(searchStart, line.cend(), sm, std::regex("(-?\\d+)"))) {

        const long number = std::stol(sm[1]);
        ret.push_back(number);

        searchStart = sm.suffix().first;
    }
    return ret;
};

long firstNumber(const std::string& line) {
    std::smatch sm;
    std::regex_search(line.cbegin(), line.cend(), sm, std::regex("(-?\\d+)"));

    const long number = std::stol(sm[1]);
    return number;
};

template<class T>
void printCollection(const T &whatever) {
    for(auto t:whatever) {
        std::cout << t << " ";
    }
    std::cout << std::endl;
}

template<class T>
void printVector(const std::vector<T> &vec) {
    for(auto t:vec) {
        std::cout << t << " ";
    }
    std::cout << std::endl;
}

template<class T>
void printMatrix(const std::vector<T> &mat) {
    for (auto &vec:mat) {
        printCollection<T>(vec);
    }
    std::cout << std::endl;
}
