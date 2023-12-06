#include <iostream>
#include <sstream>
#include <fstream>

#include <math.h>
#include <regex>
#include <string>
#include <vector>

using namespace std;

std::vector<long> getNumbers(const std::string& line) {
    smatch sm;
    std::vector<long> ret;
    string::const_iterator searchStart(line.cbegin() );
    while (regex_search(searchStart, line.cend(), sm, regex("(\\d+)"))) {

        const long number = std::stol(sm[1]);
        ret.push_back(number);

        searchStart = sm.suffix().first;
    }
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

    auto someLambda = [&]() {
    };

    std::string line;
    getline(listFile, line);
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    auto time = getNumbers(line);
    getline(listFile, line);
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    auto distance = getNumbers(line);

    long nbWays = 1;
    const size_t n = time.size();
    for (size_t i = 0; i < n; ++i) {
        // cout << time[i] << " " << distance[i] << " ... ";

        const double rad = sqrt(time[i]*time[i] - 4 * (distance[i] + 0.01));
        long sol1 = ceil((time[i] - rad) / 2);
        long sol2 = floor((time[i] + rad) / 2);
        // cout << "Sol : " << sol1 << " .. " << sol2 << endl;
        nbWays *= (sol2 - sol1 + 1);
    }

    cout << "Ways: " << nbWays << endl;

    return EXIT_SUCCESS;
}
