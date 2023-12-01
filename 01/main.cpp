#include <iostream>
#include <fstream>

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

    std::string line;
    size_t sum = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            const size_t firstDigitPos = line.find_first_of(digits);
            const size_t lastDigitPos = line.find_last_of(digits);
            if ((firstDigitPos != std::string::npos) && (lastDigitPos != std::string::npos)) {
                sum += 10 * static_cast<size_t>(line[firstDigitPos] - '0') + static_cast<size_t>(line[lastDigitPos] - '0');
            }
        }
    }

    std::cout << "Sum: " << sum << std::endl;
    return EXIT_SUCCESS;
}
