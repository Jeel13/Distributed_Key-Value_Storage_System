#include "kv_store.h"
#include "snapshot.h"

#include <sstream>

KeyValueStore::KeyValueStore(
    const std::string& walFilename
) : wal(walFilename), lastSequence(0) {
    recover();
}

void KeyValueStore::put(
    const std::string& key,
    const std::string& value
) {
    std::unique_lock<std::shared_mutex> lock(mutex);

    uint64_t sequence = wal.appendPut(key, value);

    data[key]=value;

    lastSequence=sequence;
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

    uint64_t sequence = wal.appendDelete(key);

    bool removed = data.erase(key) > 0;

    lastSequence = sequence;

    return removed;
}

void KeyValueStore::recover(){
    SnapshotData snapshot = Snapshot::load("snapshot.dat");

    {
        std::unique_lock<std::shared_mutex> lock(mutex);
        data=std::move(snapshot.data);
        lastSequence=snapshot.lastSequence;
    }

    auto entries=wal.readAll();

    std::unique_lock<std::shared_mutex> lock(mutex);

    for(const auto& entry : entries){
        if(entry.sequence<=lastSequence){
            continue;
        }

        if(entry.operation=="PUT"){
            data[entry.key]=entry.value;
        }
        else if(entry.operation=="DELETE"){
            data.erase(entry.key);
        }

        lastSequence=entry.sequence;
    }
}

void KeyValueStore::createSnapshot(
    const std::string& filename
) {
    std::unordered_map<std::string, std::string> snapshotData;
    uint64_t sequence;

    {
        std::shared_lock<std::shared_mutex> lock(mutex);

        snapshotData=data;
        sequence=lastSequence;
    }

    Snapshot::save(
        filename,
        sequence,
        snapshotData
    );
}