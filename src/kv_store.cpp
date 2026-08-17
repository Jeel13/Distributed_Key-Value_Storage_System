#include "kv_store.h"
#include <sstream>

KeyValueStore::KeyValueStore(
    const std::string& walFilename
) : wal(walFilename) {
    recover();
}

void KeyValueStore::put(
    const std::string& key,
    const std::string& value
) {
    wal.appendPut(key, value);

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
    wal.appendDelete(key);

    std::unique_lock<std::shared_mutex> lock(mutex);

    return data.erase(key) > 0;
}

void KeyValueStore::recover(){
    auto entries=wal.readAll();

    std::unique_lock<std::shared_mutex> lock(mutex);

    for(const std::string& entry : entries){
        std::stringstream stream(entry);

        std::string command;
        std::string key;
        std::string value;

        std::getline(stream, command, '|');
        std::getline(stream, key, '|');

        if(command=="PUT"){
            std::getline(stream, value, '|');

            data[key]=value;
        }
        else if(command=="DELETE"){
            data.erase(key);
        }
    }
}