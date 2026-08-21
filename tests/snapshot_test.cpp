#include <iostream>
#include <cstdio>
#include <unordered_map>

#include "snapshot.h"

int main() {

    const std::string filename= "test_snapshot.dat";

    std::remove(filename.c_str());

    std::unordered_map<std::string, std::string> data;

    data["name"]="jeel";
    data["city"]="Tempe";
    data["language"]="C++";
    data["project"]="distributed-kv";

    uint64_t sequence = 42;

    Snapshot::save(
        filename,
        sequence,
        data
    );

    std::cout<< "PASS: Snapshot created\n";

    SnapshotData loaded =
        Snapshot::load(filename);

    if (loaded.lastSequence != sequence) {
        std::cerr<< "FAIL: Expected sequence "
                << sequence
                << ", got "
                << loaded.lastSequence
                << "\n";

        return 1;
    }

    std::cout<< "PASS: Snapshot sequence is correct\n";

    if (loaded.data.size() != data.size()) {
        std::cerr<<"FAIL: Expected "
                << data.size()
                <<" entries, got "
                << loaded.data.size()
                <<"\n";

        return 1;
    }

    std::cout<<"PASS: Snapshot contains correct number of entries\n";

    for(const auto& [key, value] : data) {
        auto it = loaded.data.find(key);

        if(it == loaded.data.end()) {
            std::cerr<< "FAIL: Missing key: "
                    << key
                    << "\n";

            return 1;
        }

        if(it->second != value) {

            std::cerr<<"FAIL: Incorrect value for key: "
                    << key
                    <<"\n";

            return 1;
        }
    }

    std::cout<<"PASS: All snapshot values are correct\n";

    std::cout<< "\nLoaded snapshot:\n";

    std::cout<<"LAST_SEQUENCE|"
            << loaded.lastSequence
            <<"\n";

    for (const auto& [key, value] : loaded.data) {
        std::cout<< key
                <<"|"
                << value
                <<"\n";
    }

    std::remove(filename.c_str());

    std::cout<<"\nAll snapshot tests passed.\n";

    return 0;
}