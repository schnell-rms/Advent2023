#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <utils.h>
#include <unordered_map>

using namespace std;

const TNumber kMAX_VALUE = 4000;
const TNumber kMIN_VALUE = 1;

using TRange = std::pair<TNumber,TNumber>;
using T4Ranges = std::unordered_map<char, TRange>;

TNumber countRanges(T4Ranges &range) {
    return      (range['x'].second - range['x'].first + 1)
            *   (range['m'].second - range['m'].first + 1)
            *   (range['a'].second - range['a'].first + 1)
            *   (range['s'].second - range['s'].first + 1);
}

std::string extractRule(const std::string& line) {
    std::smatch sm;
    std::regex_search(line.cbegin(), line.cend(), sm, std::regex("([a-z]+){(.+):([a-z])([0-9a-f]{6})"));
    return sm[1].str();
};

struct SMetalPart {
    std::unordered_map<char, TNumber> values;

    SMetalPart(std::string &line) {
        std::smatch sm;
        auto numbers = allNumbers(line);
        assert(numbers.size() == 4);
        values['x'] = numbers[0];
        values['m'] = numbers[1];
        values['a'] = numbers[2];
        values['s'] = numbers[3];
    }

    TNumber sum() const {
        TNumber s = 0;
        for (auto &v:values) {
            s += v.second;
        }
        return s;
    }
};

struct SRule {
    char operand;
    TRange rangeOk;
    TRange rangeNotOk;
    std::string nextState;

    bool operator()(const SMetalPart &part) const {
        const auto it = part.values.find(operand);
        assert(it != part.values.end());
        // return (comp == '>') ? (it->second > limit) : (it->second < limit);
        return (rangeOk.first < it->second) && (it->second < rangeOk.second);
    }

    SRule(const std::string &ruleStr)
    : operand(ruleStr[0])
    , rangeOk(kMIN_VALUE, kMAX_VALUE)
    , rangeNotOk(kMIN_VALUE, kMAX_VALUE)
    {
        TNumber limit = firstNumber(ruleStr);

        if (ruleStr[1] == '<') {
            rangeOk.second = limit - 1;
            rangeNotOk.first = limit;
        } else {
            rangeOk.first = limit + 1;
            rangeNotOk.second = limit;
        }

        const size_t dp = ruleStr.find(':');
        nextState = ruleStr.substr(dp+1);
    }
};

struct SWorkFlow {
    std::string lastState;
    std::vector<SRule> rules;

    std::string process(const SMetalPart& part) const {
        for (const auto &r:rules) {
            if (r(part)) {
                return r.nextState;
            }
        }
        return lastState;
    }
};

using TAllWorkflow = std::unordered_map<std::string, SWorkFlow>;

void extractWorkflow(TAllWorkflow &allW, const std::string& line) {
    SWorkFlow s;

    size_t pos = line.find('{',0);
    const auto name = line.substr(0,pos);

    while (pos!=std::string::npos) {
        const size_t nextPos = line.find(',', pos+1);

        std::string ruleStr = line.substr(pos+1, nextPos - pos - 1);

        const size_t dp = ruleStr.find(':');
        if (dp == std::string::npos) {
            ruleStr.pop_back();
            s.lastState = ruleStr;
            break;
        }

        s.rules.emplace_back(ruleStr);
        pos = nextPos;
    }    

    allW[name] = s;
};

bool isSelected(const TAllWorkflow &allW, const SMetalPart &part) {
    std::string key = "in";
    while ((key!= "A") && (key!="R") ) {
        auto it = allW.find(key);
        assert(it!=allW.end());
        key = it->second.process(part);
    }
    return key == "A";
}

TNumber combinations(const TAllWorkflow &allW,
                     const std::string key,
                     T4Ranges ranges)
{
    if (key == "R") {
        return 0;
    }

    if (key == "A") {
        return countRanges(ranges);;
    }

    auto intersect = [](const TRange &r1, const TRange &r2) {
        return TRange{std::max(r1.first, r2.first), std::min(r1.second, r2.second)};
    };

    auto isValid = [](const TRange &r) {
        return r.first < r.second;
    };

    auto itWorkflow = allW.find(key);
    TNumber n = 0;
    bool isRangeForLastState = true;
    for (auto rule:itWorkflow->second.rules) {
        TRange memRange = ranges[rule.operand];
        TRange rangeSlice = intersect(rule.rangeOk, memRange);
        if (isValid(rangeSlice)) {
            ranges[rule.operand] = rangeSlice;
            n += combinations(allW, rule.nextState, ranges);
        }

        rangeSlice = intersect(rule.rangeNotOk, memRange);
        if (!isValid(rangeSlice)) {
            isRangeForLastState = false;
            break;
        }
        ranges[rule.operand] = rangeSlice;
    }

    if (isRangeForLastState) {
        n += combinations(allW, itWorkflow->second.lastState, ranges);
    }
    return n;
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

    auto someLambda = [&]() {
    };
    
    TAllWorkflow allW;
    std::string line;
    while(getline(listFile, line) && !line.empty()) {
        extractWorkflow(allW, line);
    }


    std::vector<SMetalPart> parts;
    while(getline(listFile, line) && !line.empty()) {
        parts.emplace_back(line);
    }

    size_t silverSum = 0;

    for (auto &p:parts) {
        if (isSelected(allW, p)) {
            silverSum += p.sum();
        }
    }

    cout << "Total silver is: " << silverSum << endl;

    T4Ranges initalRanges;
    initalRanges['x'] = {kMIN_VALUE, kMAX_VALUE};
    initalRanges['m'] = {kMIN_VALUE, kMAX_VALUE};
    initalRanges['a'] = {kMIN_VALUE, kMAX_VALUE};
    initalRanges['s'] = {kMIN_VALUE, kMAX_VALUE};

    const auto goldSum = combinations(allW,"in", initalRanges);
    cout << "Total gold is: " << goldSum << endl;

    return EXIT_SUCCESS;
}
