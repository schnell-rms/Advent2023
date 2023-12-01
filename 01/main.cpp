#include <iostream>
#include <fstream>

#include <vector>

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

    const std::string digits = "0123456789";
    const std::vector<std::string> numbers = {
        "one",
        "two",
        "three",
        "four",
        "five",
        "six",
        "seven",
        "eight",
        "nine",
    };


    std::vector<std::string> reversedNumbers;
    for (auto& nb:numbers) {
        std::string revNb = std::string(nb.rbegin(), nb.rend());
        reversedNumbers.push_back(std::move(revNb));
    }

    auto getFirstDigit = [&digits](const std::string& line, const std::vector<std::string>& numbers) {
        const size_t firstDigitPos = line.find_first_of(digits);
        size_t firstDigit = 0;
        if (firstDigitPos != std::string::npos) {
            firstDigit = line[firstDigitPos] - '0';
        }
        size_t firstPos = firstDigitPos;
        for(size_t i=0; i<numbers.size(); ++i) {
            const size_t pos = line.find(numbers[i]);
            if (pos < firstPos) {
                firstPos = pos;
                firstDigit = i+1;
            }
        }
        return firstDigit;
    };

    std::string line;
    size_t sum = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            const size_t firstDigit = getFirstDigit(line, numbers);

            std::string reversedLine = std::string(line.rbegin(), line.rend());
            const size_t lastDigit = getFirstDigit(reversedLine, reversedNumbers);
            sum += 10 * firstDigit + lastDigit;
        }
    }

    std::cout << "Sum: " << sum << std::endl;
    return EXIT_SUCCESS;
}
