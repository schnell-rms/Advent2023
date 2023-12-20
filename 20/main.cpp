#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <utils.h>
#include <vector>
#include <queue>
#include <unordered_map>
#include <numeric>

using namespace std;

const bool gCONJUNTION_DEFAULT_MEMORY = false;
const bool gFLIP_FLOP_DEFAULT_STATE = false;
const std::string gBROADCASTER = "broadcaster";
const std::string gRX_OUTPUT = "rx";

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
    bool isFlipFlopOn = gFLIP_FLOP_DEFAULT_STATE;

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

    bool isConjuntionHigh() const {
        for (auto it:conjunctionMemory) {
            if (!it.second) return false;
        }
        return true;
    }

    void clear() {
        for (auto it:conjunctionMemory) {
            it.second = gCONJUNTION_DEFAULT_MEMORY;
        }
        isFlipFlopOn = gFLIP_FLOP_DEFAULT_STATE;
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
        sendToAll(!isConjuntionHigh(), q);
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

    // ________________________________________________________
    // SILVER
    // --------------------------------------------------------
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

    // ________________________________________________________
    // GOLD
    // --------------------------------------------------------
    
    // RX has in this example only one input.
    // On the general case, each input should be treated separatelly and
    // use the one the sends a low pulse faster.
    const SModule rx = scheme[gRX_OUTPUT];
    // So, only one module as input for RX, which is a conjunction module:
    const SModule &m1 = scheme[rx.conjunctionMemory.begin()->first];
    // Take its inputs: <name , number of button presses until it gives a high pulse>
    std::unordered_map<std::string, TNumber> back2Modules;
    for (const auto &m2:m1.conjunctionMemory) {
        back2Modules[m2.first] = 0;
    }

    // Clean everything after silver:
    for (auto &module:scheme) {
        module.second.clear();
    }

    // Used for stopping the loop
    auto allSet = [&]() {
        for (const auto &c:back2Modules) {
            if (c.second == 0) return false;
        }
        return true;
    };

    TNumber n = 0;//number of button presses
    while (!allSet()) {
        // Press the button (again)
        TQPulses q;
        SModule bc = scheme[gBROADCASTER];

        q.push({"", gBROADCASTER, false});
        n++;

        while (!q.empty()) {
            SPulse p = q.front();
            q.pop();

            SModule &m = scheme[p.target];
            m.receivePulse(p, q);

            // Did a back2 module sent a high pulse?
            auto itc2 = back2Modules.find(p.source);
            if (    (back2Modules.end() != itc2)
                &&  (itc2->second == 0)
                &&  p.isHigh)
            {
                cout << itc2->first << " ===> " << n - itc2->second << endl;
                itc2->second = n;
            }
        }
    }

    size_t gold = 1;
    // Get when all back2 modules will send a high pulse to the back1 module.
    for (auto &c:back2Modules) {
        gold = std::lcm(gold,c.second);
    }

    cout << "Gold is: " << gold << endl;

    return EXIT_SUCCESS;
}
