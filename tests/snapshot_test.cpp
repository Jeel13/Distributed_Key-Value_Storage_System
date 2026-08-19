#include <iostream>

#include "kv_store.h"

int main(){
    KeyValueStore store("data.wal");

    store.createSnapshot("snapshot.dat");
    
    std::cout<<"Snapshot created\n";

    return 0;
}