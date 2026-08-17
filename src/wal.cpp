#include "wal.h"

#include <stdexcept>
#include <fstream>

WAL::WAL(const std::string& filename)
    :filename(filename) {

    file.open(
        filename,
        std::ios::out | std::ios::app
    );

    if(!file.is_open()){
        throw std::runtime_error(
            "Failed to open WAL file"
        );
    }
}

void WAL::appendPut(
    const std::string& key,
    const std::string& value
) {
    std::lock_guard<std::mutex> lock(mutex);

    file<<"PUT|"
        << key
        <<"|"
        << value
        <<"\n";

    file.flush();
}

void WAL::appendDelete(
    const std::string& key
) {
    std::lock_guard<std::mutex> lock(mutex);

    file<<"DELETE|"
        << key
        <<"\n";

    file.flush();
}

std::vector<std::string> WAL::readAll(){
    std::vector<std::string> entries;

    std::ifstream input(filename);

    if(!input.is_open()){
        return entries;
    }

    std::string line;

    while(std::getline(input, line)){
        if(!line.empty()){
            entries.push_back(line);
        }
    }

    return entries;
}