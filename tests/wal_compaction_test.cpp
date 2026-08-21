#include <iostream>
#include <fstream>
#include <cstdio>

#include "wal.h"

int main() {
    const std::string walFile="test_compaction.wal";

    std::remove(walFile.c_str());
    std::remove((walFile + ".tmp").c_str());

    WAL wal(walFile);

    wal.appendPut("name", "jeel");       
    wal.appendPut("age", "25");          
    wal.appendPut("city", "Tempe");      
    wal.appendPut("country", "USA");     
    wal.appendDelete("age");             

    auto entries = wal.readAll();

    if (entries.size() != 5) {
        std::cerr<<"FAIL: Expected 5 WAL entries, got "
                << entries.size()
                <<"\n";

        return 1;
    }

    std::cout<< "PASS: Initial WAL contains 5 entries\n";

    wal.compact(3);

    entries = wal.readAll();

    if (entries.size() != 2) {
        std::cerr<< "FAIL: Expected 2 entries after compaction, got "
                << entries.size()
                << "\n";

        return 1;
    }

    if(entries[0].sequence!=4 || entries[1].sequence!=5) {
        std::cerr<< "FAIL: Incorrect sequences after compaction\n";
        return 1;
    }

    std::cout<<"PASS: WAL contains only entries after sequence 3\n";

    if(entries[0].operation != "PUT" || entries[0].key != "country" || entries[0].value != "USA") {
        std::cerr<< "FAIL: Incorrect PUT entry after compaction\n";

        return 1;
    }

    if(entries[1].operation != "DELETE" || entries[1].key != "age") {
        std::cerr<<"FAIL: Incorrect DELETE entry after compaction\n";

        return 1;
    }

    std::cout<< "PASS: Remaining WAL entries are correct\n";

    std::cout<<"\nFinal WAL:\n";

    for(const auto& entry : entries) {
        std::cout<< entry.sequence
                <<"|"
                << entry.operation
                <<"|"
                << entry.key;

        if(entry.operation=="PUT") {
            std::cout<<"|"
                    << entry.value;
        }

        std::cout<<"\n";
    }

    std::remove(walFile.c_str());
    std::remove((walFile + ".tmp").c_str());

    std::cout<<"\nAll WAL compaction tests passed.\n";

    return 0;
}