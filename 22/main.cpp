#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <utils.h>
#include <unordered_map>
#include <algorithm>

using namespace std;

struct Point2D {
    TNumber x,y;
};

struct Point3D : public Point2D {
    TNumber z;
};

struct SSquare {
    Point2D from, to;
    TNumber key() {
        // all positives
        assert((to.x >= from.x) && (from.x >= 0));
        assert((to.y >= from.y) && (from.y >= 0));
        return (from.x << 48) | (from.y<<32) | (to.x << 16) | (to.y);
    }
};

// <Z level, bricks on that level> 
// Could store just the keys in an unordered_set. Whatever.
using TLevels = std::unordered_map<TNumber, std::unordered_map<TNumber,SSquare>>;

struct SBrick {
    Point3D from, to;
    SBrick(const std::string &line) {
        auto numbers = allNumbers(line);
        assert(numbers.size() == 6);
        from = {numbers[0], numbers[1], numbers[2]};
        to = {numbers[3], numbers[4], numbers[5]};
        // particular input:
        assert(numbers[0] <= numbers[3]);
        assert(numbers[1] <= numbers[4]);
        assert(numbers[2] <= numbers[5]);
    }

    bool intersects(const SSquare &other) const {
        const bool isXOutside =     (from.x > other.to.x)
                                ||  (to.x < other.from.x);
        const bool isYOutside =     (from.y > other.to.y)
                                ||  (to.y < other.from.y);
        return !isXOutside && !isYOutside;
    }

    bool operator<(const SBrick &other) const {
        return from.z < other.from.z;
    }

    void addLevels(TLevels &levels) const {
        SSquare square{from, to};
        for(TNumber z = from.z; z<=to.z; z++) {
            levels[z][square.key()] = square;
        }
    }

    void removeLevels(TLevels &levels) const {
        SSquare square{from, to};
        for(TNumber z = from.z; z<=to.z; z++) {
            levels[z].erase(square.key());
            if (levels[z].empty()) {
                levels.erase(z);
            }
        }
    }

    bool fall(TLevels &levels, bool updateZ){
        TNumber z = from.z - 1;

        for (; z>=1; z--) {
            auto it = levels.find(z);
            if (it == levels.end()) {
                continue;
            }

            bool isSupportFound = false;
            auto squares = it->second;
            for (auto &s:squares) {
                if (intersects(s.second)) {
                    isSupportFound = true;
                    break;
                }
            }

            if (isSupportFound) {
                break;
            }
        }

        if (z != from.z - 1) {
            const TNumber zFall = from.z - (z + 1);
            removeLevels(levels);
            from.z -= zFall;
            to.z -= zFall;
            addLevels(levels);
            if (!updateZ) {
                // put the brick back, only the levels are adjusted according to the fall:
                from.z += zFall;
                to.z += zFall;
            }
            return true;
        }
        return false;
    }

    bool wouldFall(TLevels &levels) const {
        const TNumber zBelow = from.z - 1;
        if (zBelow == 0) {
            return false;
        }
        
        auto it = levels.find(zBelow);
        if (it == levels.end()) {
            return true;
        }

        auto squares = it->second;
        for (auto &s:squares) {
            if (intersects(s.second)) {
                return false;
            }
        }
        return true;
    }

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

    std::vector<SBrick> bricks;
    TLevels levels;

    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            bricks.emplace_back(line);
            bricks.back().addLevels(levels);
        }
    }

    std::sort(bricks.begin(), bricks.end());

    // Fall all bricks
    for (auto &b : bricks) {
        b.fall(levels, true);
    }
    
    // ________________________________________________________
    // SILVER
    // --------------------------------------------------------

    TNumber silver = 0;
    for (size_t i=0; i < bricks.size(); ++i) {
        bricks[i].removeLevels(levels);
        bool somethingWouldFall = false;
        for (size_t j = i+1; j < bricks.size(); j++) {
            if (bricks[j].from.z == bricks[i].from.z) {
                continue;
            }
            if (bricks[j].wouldFall(levels)) {
                somethingWouldFall = true;
                break;
            }
        }
        bricks[i].addLevels(levels);
        silver += !somethingWouldFall;
    }

    cout << "Silver: " << silver << endl;

    // ________________________________________________________
    // GOLD
    // --------------------------------------------------------
    TNumber gold = 0;
    for (size_t i=0; i < bricks.size(); ++i) {
        TLevels tempLevels = levels;
        bricks[i].removeLevels(tempLevels);
        // Fall all bricks - actually just the levels, not the bricks, to avoid a temporary bricks' copy:
        for (size_t j = i+1; j < bricks.size(); j++) {
            gold += bricks[j].fall(tempLevels, false);
        }
    }
    
    cout << "Gold: " << gold << endl;
    return EXIT_SUCCESS;
}
