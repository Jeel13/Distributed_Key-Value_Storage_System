#include "kv_store.h"

void KeyValueStore::put(
    const std::string& key,
    const std::string& value
) {
    data[key]=value;
}

std::optional<std::string> KeyValueStore::get(
    const std::string& key
) {
    auto it=data.find(key);

    if(it==data.end()){
        return std::nullopt;
    }

    return it->second;
}

bool KeyValueStore::remove(
    const std::string& key
) {
    return data.erase(key) > 0;
}