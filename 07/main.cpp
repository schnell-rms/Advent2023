#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <set>

using namespace std;

using THand = std::pair<std::string, long>;

long getNumber(const std::string& line) {
    smatch sm;
    regex_search(line.cbegin(), line.cend(), sm, regex("(\\d+)"));

    const long number = std::stol(sm[1]);
    return number;
};

int getCardOrder(char c) {
    const std::string order = "AKQJT98765432";
    return order.find(c);
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


    auto getType = [&](const THand& hand) {
        unordered_map<char, int> cardCount;
        for (char c:hand.first) {
            cardCount[c]++;
        }

        set<int> count;
        for (auto a:cardCount) {
            count.insert(a.second);
        }


        if (cardCount.size()==5)
            return 1;

        if (cardCount.size()==4)
            return 2;


        if (cardCount.size()==3) {
            if (*(count.rbegin()) == 3)
                return 4;
            
            return 3;
        }

        if (cardCount.size()==2) {
            if (*(count.rbegin()) == 4)
                return 6;
            return 5;
        }
        return 7;

    };

    auto order = [&](const THand& hand1, const THand& hand2) {
        int type1 = getType(hand1);
        int type2 = getType(hand2);
        if (type1 == type2) {
            for (size_t i=0; i<5; i++) {
                if (hand1.first[i] == hand2.first[i]) {
                    continue;
                }
                return getCardOrder(hand2.first[i]) < getCardOrder(hand1.first[i]);
            }
        }
        
        return type2 > type1;
    };

    std::vector<THand> allHands;

    std::string line;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            THand hand;
            hand.first = line.substr(0,5);
            hand.second = getNumber(line.substr(5));

            cout << "Hand " << hand.first << " " << hand.second << endl;
            allHands.push_back(std::move(hand));
            
        }
    }


    std::sort(allHands.begin(), allHands.end(), order);

    cout << "PRODUCTS:" << endl;
    long total = 0;
    for (long i=1; i<= allHands.size(); ++i) {
        cout << "HAND " << allHands[i-1].first << endl;
        cout << "Prodcut " << i << " * " <<  allHands[i-1].second << endl;
        total += i * allHands[i-1].second;
    }

    cout << "Total " << total << endl;

    return EXIT_SUCCESS;
}
