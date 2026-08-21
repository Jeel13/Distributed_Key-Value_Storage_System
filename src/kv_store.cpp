#include "kv_store.h"
#include "snapshot.h"

#include <sstream>

KeyValueStore::KeyValueStore(
    const std::string& walFilename,
    const std::string& snapshotFilename
) : wal(walFilename), 
    snapshotFilename(snapshotFilename),
    lastSequence(0),
    operationsSinceSnapshot(0),
    running(true) {

    recover();

    snapshotThread = std::thread(
        &KeyValueStore::snapshotLoop,
        this
    );
}

void KeyValueStore::put(
    const std::string& key,
    const std::string& value
) {
    bool shouldNotify=false;

    {
        std::unique_lock<std::shared_mutex> lock(mutex);
    
        uint64_t sequence = wal.appendPut(key, value);
    
        data[key]=value;
    
        lastSequence=sequence;
    
        operationsSinceSnapshot++;

        if(operationsSinceSnapshot>=SNAPSHOT_INTERVAL){
            shouldNotify=true;
        }
    }

    if(shouldNotify){
        snapshotCondition.notify_one();
    }
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
    bool shouldNotify=false;
    bool removed;

    {
        std::unique_lock<std::shared_mutex> lock(mutex);
    
        uint64_t sequence = wal.appendDelete(key);
    
        removed = data.erase(key) > 0;
    
        lastSequence = sequence;
    
        operationsSinceSnapshot++;
    
        if(operationsSinceSnapshot>=SNAPSHOT_INTERVAL){
            shouldNotify=true;
        }
    }

    if(shouldNotify){
        snapshotCondition.notify_one();
    }


    return removed;
}

void KeyValueStore::recover(){
    SnapshotData snapshot = Snapshot::load(snapshotFilename);

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

    wal.compact(sequence);

    uint64_t currentSequence;

    {
        std::unique_lock<std::shared_mutex> lock(mutex);

        currentSequence=lastSequence;
    }

    {
        std::unique_lock<std::shared_mutex> lock(mutex);

        operationsSinceSnapshot=currentSequence-sequence;
    }
}

void KeyValueStore::snapshotLoop(){
    while(running){
        std::unique_lock<std::mutex> lock(snapshotMutex);

        snapshotCondition.wait(
            lock,
            [this]() {
                return !running || operationsSinceSnapshot>=SNAPSHOT_INTERVAL;
            }
        );

        if(!running){
            break;
        }

        lock.unlock();
        
        createSnapshot(snapshotFilename);
    }
}

KeyValueStore::~KeyValueStore(){
    running=false;

    snapshotCondition.notify_one();

    if(snapshotThread.joinable()){
        snapshotThread.join();
    }
}