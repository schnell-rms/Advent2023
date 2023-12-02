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

    const int kMaxRed = 12;
    const int kMaxGreen = 13;
    const int kMaxBlue = 14;

    auto checkGame = [&](const std::string &game) {
        std::string grab;
        std::istringstream gameStream(game);
        while (std::getline(gameStream, grab, ';'))
        {
            int red = 0, blue = 0, green = 0;
            smatch sm;
            if (regex_search(grab, sm, regex("(\\d+) blue"))) {
                blue = std::stoi(sm[1]);
                if (blue > kMaxBlue) {
                    return false;
                }
            }

            if (regex_search(grab, sm, regex("(\\d+) red"))) {
                red = std::stoi(sm[1]);
                if (red > kMaxRed) {
                    return false;
                }

            }

            if (regex_search(grab, sm, regex("(\\d+) green"))) {
                green = std::stoi(sm[1]);
                if (green > kMaxGreen) {
                    return false;
                }
            }
        }
        return true;
    };

    std::string line;
    int gameId = 1;
    int gameSum = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            size_t startPos = 0;//line.find(':');
            // Could extract gameId using (regex_search(line, sm, regex("Game (\\d+)"))) 
            // Rely on counting lines, instead:
            if (checkGame(line.substr(startPos + 1))) {
                gameSum += gameId;
            }

            gameId++;
        }
    }

    cout << "Sum of possible games: " << gameSum << endl;
    return EXIT_SUCCESS;
}
