#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <cstdint>
#include <string>
#include <unordered_map>

struct SnapshotData {
    uint64_t lastSequence;
    std::unordered_map<std::string, std::string> data;
};

class Snapshot{
public:
    static void save(
        const std::string& filename,
        uint64_t lastSequence,
        const std::unordered_map<std::string, std::string>& data
    );

    static SnapshotData load(
        const std::string& filename
    );
};

#endif