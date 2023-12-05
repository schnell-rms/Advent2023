#include <iostream>
#include <sstream>
#include <fstream>

#include <regex>
#include <string>
#include <vector>

using namespace std;

std::vector<long> getNumbers(const std::string& line) {
    smatch sm;
    std::vector<long> ret;
    string::const_iterator searchStart(line.cbegin() );
    while (regex_search(searchStart, line.cend(), sm, regex("(\\d+)"))) {

        const long number = std::stol(sm[1]);
        ret.push_back(number);

        searchStart = sm.suffix().first;
    }
    return ret;
};

struct SRange {
    SRange(long start, long end)
    : _start(start)
    , _end(end)
    {
        assert(end > start);
    }
    long _start;
    long _end;

    void convert(long deltaConversion) {
        _start += deltaConversion;
        _end += deltaConversion;
    }

    void collectIntersection(SRange& other, std::vector<SRange> &intersection, std::vector<SRange> &outside, long deltaConversion) const {
        const long intersectionStart = std::max(_start, other._start);
        const long intersectionEnd =  std::min(_end, other._end);
        if (intersectionEnd > intersectionStart) {
            if (other._start < _start) {
                // Left
                outside.emplace_back(other._start, _start);
            }

            if (other._end > _end) {
                // Right
                outside.emplace_back(_end, other._end);
            }

            SRange common(intersectionStart, intersectionEnd);
            common.convert(deltaConversion);
            intersection.push_back(std::move(common));
        }
        else {
            outside.push_back(other);
        }
    }
};

struct SMap {
    SMap(long dest, long start, long len)
    :_dest(dest, dest + len)
    ,_source(start, start + len)
    {}
    SRange _dest;
    SRange _source;

    long convert(long input) {
        if ((input >= _source._start) && (input < _source._end))
        {
            return input - _source._start + _dest._start;
        }
        return -1;
    }

    void convert(SRange& input, std::vector<SRange>& converted, std::vector<SRange>& notConverted) const {
        // Pass the input range through this mapping level and collect the results
        _source.collectIntersection(input, converted, notConverted, _dest._start - _source._start);
    }
};

SMap readMap(const std::string& line) {
    std::vector<long> vals = getNumbers(line);
    return SMap(vals[0], vals[1], vals[2]);
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

    // get seeds
    std::string line;
    getline(listFile, line);
    std::vector<long> seeds = getNumbers(line);

    std::vector<SRange> seedRanges;
    for (size_t i=0; i<seeds.size(); i+=2) {
        seedRanges.emplace_back(SRange(seeds[i], seeds[i] + seeds[i+1]));
    }

    // GET ALL THE MAPS:
    std::vector<std::vector<SMap>> maps(7);
    long mapIdx = -1;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                mapIdx++;
                continue;
            }

            maps[mapIdx].emplace_back(readMap(line));
        }
    }

    auto getLocation = [&](long seed) {
        for (std::vector<SMap>& ms:maps) {
            long dest = -1;
            size_t idx = 0;
            while ((dest == -1) && (idx < ms.size())) {
                dest = ms[idx].convert(seed);
                idx++;
            }
            
            if (dest != -1) {
                seed = dest;
            }
//            else keep the same value
        }

        return seed;
    };


    auto getRangeLocation = [&](std::vector<SRange>& seedRanges) {
        // Loop on maps' levels
        for (const std::vector<SMap>& msLevel:maps) {
            std::vector<SRange> converted;// convertion by this mapping level
            // Convert seed ranges:
            for (auto& oneSeedRange:seedRanges) {
                // Loop on maps of this level
                std::vector<SRange> notConverted;
                notConverted.push_back(oneSeedRange);
                for (const SMap& ms:msLevel) {
                    std::vector<SRange> nextNotConverted;
                    for (auto& notYetConvertedRange: notConverted) {
                        ms.convert(notYetConvertedRange, converted, nextNotConverted);
                    }
                    notConverted.swap(nextNotConverted);
                    if (notConverted.empty()) {
                        break;
                    }
                }

                converted.insert(converted.end(),notConverted.begin(),notConverted.end());
            }
            // Go to next mapping level:
            seedRanges.swap(converted);
        }

        return seedRanges;
    };

    // GET THE LOCATIONS, first star:
    long minLocation = std::numeric_limits<long>::max();
    for (long oneSeed : seeds) {
        const long location = getLocation(oneSeed);
        if (location < minLocation) {
            minLocation = location;
        }
    }

    cout << "Min Location: " << minLocation << endl;

    // GET THE LOCATIONS with seed ranges:
    minLocation = std::numeric_limits<long>::max();
    auto locationRanges = getRangeLocation(seedRanges);

    for (auto& oneLocationRange : locationRanges) {
        if (oneLocationRange._start < minLocation) {
            minLocation = oneLocationRange._start;
        }
    }

    cout << "Min Location RANGES: " << minLocation << endl;
    return EXIT_SUCCESS;
}
