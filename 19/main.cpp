#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <utils.h>
#include <unordered_map>

using namespace std;

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
    char comp;
    TNumber limit;
    std::string nextState;

    bool operator()(const SMetalPart &part) const {
        const auto it = part.values.find(operand);
        assert(it != part.values.end());
        return (comp == '>') ? (it->second > limit) : (it->second < limit);
    }

    SRule(const std::string &ruleStr)
    : operand(ruleStr[0])
    , comp(ruleStr[1])
    {
        limit = firstNumber(ruleStr);
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

    cout << "Total sum is: " << silverSum << endl;

    return EXIT_SUCCESS;
}
