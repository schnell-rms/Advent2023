#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <utils.h>
#include <vector>
#include <queue>
#include <unordered_map>

using namespace std;

const bool gCONJUNTION_DEFAULT_MEMORY = false;
const std::string gBROADCASTER = "broadcaster";

struct SPulse {
    std::string source;
    std::string target;
    bool isHigh;
};

using TQPulses=std::queue<SPulse>;

struct SModule {
    std::string name;
    char type = 0;//b, % or &
    std::vector<std::string> targets;
    std::unordered_map<std::string, bool> conjunctionMemory;
    bool isFlipFlopOn = false;

    void receivePulse(const SPulse &pulse, TQPulses &q) {
        switch (type)
        {
        case 'b':
            return sendToAll(pulse.isHigh, q);
        case '%':
            return pulseFlipFlop(pulse.isHigh, q);
        case '&':
            return pulseConjunction(pulse, q);
        default:
            //simply output
            return;
        }
    }

private:
    void sendToAll(bool isHigh,  TQPulses &q) {
        for (auto tname:targets) {
            q.push({name, tname, isHigh});
        }        
    }

    void pulseFlipFlop(bool isHigh, TQPulses &q) {
        if (isHigh) {
            return;
        }
        
        isFlipFlopOn = !isFlipFlopOn;
        sendToAll(isFlipFlopOn, q);
    }
    
    void pulseConjunction(const SPulse &p, TQPulses &q) {
        conjunctionMemory[p.source] = p.isHigh;
        bool allHigh = true;
        for (auto it:conjunctionMemory) {
            allHigh = allHigh && it.second;
        }
        sendToAll(!allHigh, q);
    }
};

using TScheme = std::unordered_map<std::string, SModule>;

void addModule(const std::string &line, TScheme &scheme) {
    std::smatch sm;
    std::string::const_iterator searchStart(line.cbegin());
    
    std::regex_search(searchStart, line.cend(), sm, std::regex("([a-z]+) "));
    searchStart = sm.suffix().first;
    const std::string name = sm[1].str();
    
    SModule &module = scheme[name];
    module.name = name;
    module.type = line[0];
    assert(module.type == 'b' || module.type == '%' || module.type == '&');

    while (std::regex_search(searchStart, line.cend(), sm, std::regex(" ([a-z]+)"))) {
        const std::string targetName = sm[1].str();
        module.targets.push_back(targetName);
        scheme[targetName].conjunctionMemory[name] = gCONJUNTION_DEFAULT_MEMORY;
        searchStart = sm.suffix().first;
    }
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
    
    TScheme scheme;

    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            addModule(line, scheme);
        }
    }

    TNumber highPulseCounter = 0;
    TNumber lowPulseCounter = 0;
    
    for (size_t i=0; i< 1000; i++) {
        TQPulses q;
        SModule bc = scheme[gBROADCASTER];

        q.push({"", gBROADCASTER, false});

        while (!q.empty()) {
            SPulse p = q.front();
            q.pop();
            highPulseCounter += p.isHigh;
            lowPulseCounter += !p.isHigh;

            SModule &m = scheme[p.target];
            m.receivePulse(p, q);
        }
    }

    const auto silver = lowPulseCounter * highPulseCounter;
    cout << "Total silver is: " << silver << endl;
    return EXIT_SUCCESS;
}
