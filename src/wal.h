#ifndef WAL_H
#define WAL_H

#include <fstream>
#include <mutex>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

struct WALEntry {
    uint64_t sequence;
    std::string operation;
    std::string key;
    std::string value;
};

class WAL {
private:
    std::ofstream file;
    std::mutex mutex;
    std::string filename;

    uint64_t nextSequence;

    std::vector<WALEntry> readAllUnlocked();

public:
    explicit WAL(const std::string& filename);

    uint64_t appendPut(
        const std::string& key,
        const std::string& value
    );

    uint64_t appendDelete(
        const std::string& key
    );

    std::vector<WALEntry> readAll();

    uint64_t getLastSequence() const;

    void compact(uint64_t sequence);
};

#endif