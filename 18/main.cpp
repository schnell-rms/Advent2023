#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <utils.h>

using namespace std;

enum EDirection {
    eRight = 0,
    eDown,
    eLeft,
    eUp,
    eNoDirection
};

std::string extractHexa(const std::string& line) {
    std::smatch sm;
    std::regex_search(line.cbegin(), line.cend(), sm, std::regex("#([0-9a-f]{6})"));
    return sm[1].str();
};

TNumber decimal(const std::string &hexa) {
    TNumber n = 0;
    for (char c:hexa) {
        const TNumber d = (c >= 'a') ? (c - 'a' + 10) : (c - '0');
        n <<= 4;
        n |= d;
    }
    return n;
}

struct SIntruction {
    TNumber steps;
    EDirection dir;

    SIntruction(const std::string& line, bool isGold) {
        if (isGold) {
            auto s = extractHexa(line);
            dir = static_cast<EDirection>(s.back() - '0');
            s.pop_back();
            steps = decimal(s);
        } else {
            steps = firstNumber(line);
            switch (line[0])
            {
            case 'R':
                dir = eRight;
                break;
            case 'L':
                dir = eLeft;
                break;
            case 'U':
                dir = eUp;
                break;
            case 'D':
                dir = eDown;
                break;
            default:
                assert(false);
                break;
            }
        }
    }
};

using TInstructions = std::vector<SIntruction>; 
using TDigPlan = std::vector<std::pair<TNumber, TNumber>>; // vector of [i][j]

TDigPlan createPlan(const TInstructions& instructions) {
    TNumber i = 0, j = 0;

    TDigPlan plan;
    plan.emplace_back(make_pair(i,j));

    for (const auto& line:instructions) {
        const long &steps = line.steps;
        switch (line.dir)
        {
        case eRight:
            j = j + steps;
            break;
        case eLeft:
            j = j - steps;
            break;
        case eUp:
            i = i - steps;
            break;
        case eDown:
            i = i + steps;
            break;
        default:
            assert(false);
            break;
        }

        plan.emplace_back(make_pair(i,j));
    }

    return plan;
}

size_t area(const TDigPlan &plan) {
    // Imagine an infinite checkboard. Each cell has its integer coordinates.
    // Now imagine floating points coordinates: the integer coordintes are in the 
    // middle of the checkboard cells!

    // Add / substract square-areas from each coordinate to the origin, while going along the countour.
    // The result will be the total area inside the floating-numbers countour.
    // It works because all countour-sides are either horizontal or vertical.
    TNumber countourArea = 0;//number of checkboard cells
    TNumber totalArea = plan[0].first * plan[0].second;//first square added
    TNumber sign = -1;//next one substracted
    for (size_t k=1; k<plan.size(); ++k) {
        TNumber chunkArea = sign * (plan[k].first * plan[k].second);
        sign *= -1;
        countourArea +=     abs(plan[k].first - plan[k-1].first)
                         +  abs(plan[k].second - plan[k-1].second);
        totalArea += chunkArea;
    }
    // totalArea - is the area inside the floating-number coordintes countour.
    // For the requested area, we have to add for each cell half of its area: 
    // half of it is already included in totalArea.
    // To add the other half, we add half of the countout area to total area.
    // ... But one cell area still has to be added!
    // Imagine the countour of a rectangle: for each of its 4 corner-cells,
    //  totalArea contains only on quarter of each cell. 
    // So adding half a contour-area is not enough: 4 more quarters have to be 
    // added, i.e. one cell area in total.
    // 
    // With more complex countours there will be more corner-cells.
    // Some will get only 1 quarter inside totalArea, while others will get 3 quarters inside.
    // But overall - since the countour is closed, there will always be 4 quarters of cells
    // which overall are outside totalArea: the one defined by the centers of the checkboard cells.
    return abs(totalArea) + countourArea/2  + 1;
}

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

    TInstructions goldInstructions;
    TInstructions silverInstructions;
    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            // TODO: could be space optimized by calling here some methods
            // to build directly TDigPlan entities. Whatever.
            goldInstructions.emplace_back(line,true);
            silverInstructions.emplace_back(line,false);
        }
    }

    TDigPlan silverPlan = createPlan(silverInstructions);
    auto silver = area(silverPlan);
    cout << "Silver area " <<  silver << endl;
    
    TDigPlan goldPlan = createPlan(goldInstructions);
    auto gold = area(goldPlan);
    cout << "Gold area " <<  gold << endl;

    return EXIT_SUCCESS;
}
