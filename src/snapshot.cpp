#include "snapshot.h"

#include <fstream>
#include <stdexcept>
#include <sstream>

void Snapshot::save(
    const std::string& filename,
    uint64_t lastSequence,
    const std::unordered_map<std::string, std::string>& data
) {
    std::ofstream file(filename);

    if(!file.is_open()) {
        throw std::runtime_error(
            "Failed to open snapshot file"
        );
    }

    file<<"LAST_SEQUENCE|"
        << lastSequence
        <<"\n";

    for(const auto& entry : data){
        file<< entry.first
            <<"|"
            << entry.second
            <<"\n";
    }
}

SnapshotData Snapshot::load(const std::string& filename){
    SnapshotData result;

    result.lastSequence=0;

    std::ifstream file(filename);

    if(!file.is_open()){
        return result;
    }

    std::string line;

    while(std::getline(file, line)){
        if(line.empty()){
            continue;
        }

        std::stringstream stream(line);

        std::string first;
        std::string second;

        std::getline(stream, first, '|');
        std::getline(stream, second);

        if(first == "LAST_SEQUENCE"){
            result.lastSequence=std::stoull(second);
        } else {
            result.data[first]=second;
        }
    }

    return result;
}