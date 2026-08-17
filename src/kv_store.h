#ifndef KV_STORE_H
#define KV_STORE_H

#include <string>
#include <unordered_map>
#include <optional>

class KeyValueStore {
private:
    std::unordered_map<std::string, std::string> data;

public:
    void put(const std::string& key, const std::string& value);

    std::optional<std::string> get(const std::string& key);

    bool remove(const std::string& key);
};

#endif