#ifndef KV_STORE_H
#define KV_STORE_H

#include <string>
#include <unordered_map>
#include <optional>
#include <shared_mutex>
#include <atomic>
#include <condition_variable>
#include <thread>

#include "wal.h"

class KeyValueStore {
private:
    std::unordered_map<std::string, std::string> data;

    std::shared_mutex mutex;

    WAL wal;

    uint64_t lastSequence;

    static constexpr uint64_t SNAPSHOT_INTERVAL = 100;

    uint64_t operationsSinceSnapshot;

    std::thread snapshotThread;

    std::atomic<bool> running;

    std::condition_variable snapshotCondition;
    std::mutex snapshotMutex;

    std::string snapshotFilename;

    void snapshotLoop();

    void recover();


public:
    explicit KeyValueStore(
        const std::string& walFilename,
        const std::string& snapshotFilename
    );

    void put(
        const std::string& key, 
        const std::string& value
    );

    std::optional<std::string> get(
        const std::string& key
    );

    bool remove(
        const std::string& key
    );

    void createSnapshot(
        const std::string& filename
    );

    ~KeyValueStore();
};

#endif