#include "wal.h"

#include <stdexcept>
#include <fstream>
#include <sstream>

WAL::WAL(const std::string& filename)
    :filename(filename),
     nextSequence(1) {

    file.open(
        filename,
        std::ios::out | std::ios::app
    );

    if(!file.is_open()){
        throw std::runtime_error(
            "Failed to open WAL file"
        );
    }

    auto entries = readAll();

    if(!entries.empty()){
        nextSequence=entries.back().sequence + 1;
    }
}

uint64_t WAL::appendPut(
    const std::string& key,
    const std::string& value
) {
    std::lock_guard<std::mutex> lock(mutex);

    uint64_t sequence = nextSequence++;

    file<< sequence
        <<"|PUT|"
        << key
        <<"|"
        << value
        <<"\n";

    file.flush();

    return sequence;
}

uint64_t WAL::appendDelete(
    const std::string& key
) {
    std::lock_guard<std::mutex> lock(mutex);

    uint64_t sequence = nextSequence++;

    file<< sequence
        <<"|DELETE|"
        << key
        <<"\n";

    file.flush();

    return sequence;
}

std::vector<WALEntry> WAL::readAllUnlocked(){

    std::vector<WALEntry> entries;

    std::ifstream input(filename);

    if(!input.is_open()){
        return entries;
    }

    std::string line;

    while(std::getline(input, line)){
        if(line.empty()){
            continue;
        }

        std::stringstream stream(line);

        std::string sequenceString;
        std::string operation;
        std::string key;
        std::string value;

        std::getline(stream, sequenceString, '|');

        std::getline(stream, operation, '|');

        std::getline(stream, key, '|');

        if(operation=="PUT"){
            std::getline(stream, value, '|');
        }

        try {
            uint64_t sequence = std::stoull(sequenceString);

            entries.push_back({
                sequence, operation, key, value
            });
        } catch (...) {
            //pending implementation for malformed WAL entries
        }
    }

    return entries;
}

std::vector<WALEntry> WAL::readAll(){
    std::lock_guard<std::mutex> lock(mutex);

    return readAllUnlocked();
}

uint64_t WAL::getLastSequence() const {
    return nextSequence - 1;
}

void WAL::compact(uint64_t sequence){
    std::lock_guard<std::mutex> lock(mutex);

    auto entries=readAllUnlocked();

    std::string tempFilename = filename + ".tmp";

    std::ofstream tempFile(
        tempFilename,
        std::ios::trunc
    );

    if(!tempFile.is_open()){
        throw std::runtime_error(
            "Failed to open temporary WAL file"
        );
    }

    for(const auto& entry: entries){
        if(entry.sequence<=sequence){
            continue;
        }

        tempFile<< entry.sequence
                <<"|"
                << entry.operation
                <<"|"
                << entry.key;
            
        if(entry.operation == "PUT"){
            tempFile<<"|"
                    << entry.value;
        }
                
        tempFile<<"\n";
    }

    tempFile.flush();
    tempFile.close();

    file.close();

    if(std::rename(tempFilename.c_str(), filename.c_str())!=0){
        throw std::runtime_error(
            "Failed to replace WAL"
        );
    }

    file.open(
        filename,
        std::ios::out | std::ios::app
    );

    if(!file.is_open()){
        throw std::runtime_error(
            "Failed to reopen WAL after compaction"
        );
    }
}