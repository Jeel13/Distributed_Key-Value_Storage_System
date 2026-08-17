#include "kv_store.h"

void KeyValueStore::put(
    const std::string& key,
    const std::string& value
) {
    std::unique_lock<std::shared_mutex> lock(mutex);

    data[key]=value;
}

std::optional<std::string> KeyValueStore::get(
    const std::string& key
) {
    std::shared_lock<std::shared_mutex> lock(mutex);

    auto it=data.find(key);

    if(it==data.end()){
        return std::nullopt;
    }

    return it->second;
}

bool KeyValueStore::remove(
    const std::string& key
) {
    std::unique_lock<std::shared_mutex> lock(mutex);

    return data.erase(key) > 0;
}