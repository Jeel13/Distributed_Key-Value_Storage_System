#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cstdio>

#include "kv_store.h"

int main() {
    const std::string walFile="concurrency_test.wal";
    const std::string snapshotFile="concurrency_test.snapshot";

    std::remove(walFile.c_str());
    std::remove(snapshotFile.c_str());

    KeyValueStore store(walFile, snapshotFile);

    const int threadCount=10;
    const int operationsPerThread=100;

    std::vector<std::thread> threads;

    for(int i=0; i<threadCount; i++){
        threads.emplace_back(
            [&store, i, operationsPerThread](){
                std::string key="key_"+std::to_string(i);

                for(int j=0; j<operationsPerThread; j++){
                    std::string value="value_"+std::to_string(j);

                    store.put(key, value);
                }
            }
        );
    }

    for(auto& thread: threads){
        thread.join();
    }

    for(int i=0; i<threadCount; i++){
        std::string key="key_"+std::to_string(i);

        auto value = store.get(key);

        if(!value){
            std::cerr<<"FAIL: Missing key "
                << key
                <<"\n";

            return 1;
        }

        if(*value!="value_99") {
            std::cerr<<"FAIL: Incorrect value for "
                    << key
                    <<". Expected value_99, got "
                    << *value
                    <<"\n";

            return 1;
        }
    }

    std::cout<<"PASS: All concurrent writes verified\n";

    threads.clear();

    for(int i=0; i<threadCount; i++){
        threads.emplace_back(
            [&store, i]() {
                std::string key="key_" + std::to_string(i);

                for(int j=0; j<100; j++){
                    auto value=store.get(key);

                    if(!value || *value!="value_99"){
                        std::cerr<<"FAIL: Concurrent GET failed for "
                                <<key
                                <<"\n";

                        return;
                    }
                }
            }
        );
    }

    for(auto& thread: threads){
        thread.join();
    }

    std::cout<<"PASS: All concurrent reads verified\n";

    threads.clear();

    for(int i=0; i<threadCount; i++){
        threads.emplace_back(
            [&store, i]() {
                std::string key="key_"+std::to_string(i);

                store.remove(key);
            }
        );
    }

    for(auto& thread: threads){
        thread.join();
    }

    for(int i=0; i<threadCount; i++) {
        std::string key ="key_" + std::to_string(i);

        auto value=store.get(key);

        if(value){
            std::cerr<<"FAIL: Key was not deleted: "
                    << key
                    <<"\n";

            return 1;
        }
    }

    std::cout<< "PASS: All concurrent deletes verified\n";

    std::remove(walFile.c_str());
    std::remove(snapshotFile.c_str());

    std::cout<< "\nAll concurrency tests passed.\n";

    return 0;
}