#include <iostream>
#include <thread>
#include <vector>

#include "kv_store.h"

int main() {
    KeyValueStore store("concurrency_test.wal", "testSnapshot.dat");

    std::vector<std::thread> threads;

    for(int i=0; i<10; i++){
        threads.emplace_back(
            [&store, i](){
                for(int j=0; j<100; j++){
                    std::string key="key_"+std::to_string(i);
                    std::string value="value_"+std::to_string(j);

                    store.put(key, value);
                }
            }
        );
    }

    for(auto& thread: threads){
        thread.join();
    }

    for(int i=0; i<10; i++){
        std::string key="key_"+std::to_string(i);

        auto value = store.get(key);

        if(value) {
            std::cout<< key
                    <<"="
                    << *value
                    <<"\n";
        }
    }

    return 0;
}