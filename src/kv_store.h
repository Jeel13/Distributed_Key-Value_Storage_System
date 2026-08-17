#ifndef KV_STORE_H
#define KV_STORE_H

#include <string>
#include <unordered_map>
#include <optional>
#include <shared_mutex>

#include "wal.h"

class KeyValueStore {
private:
    std::unordered_map<std::string, std::string> data;

    std::shared_mutex mutex;

    WAL wal;

    void recover();


public:
    explicit KeyValueStore(
        const std::string& walFilename
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

};

#endif