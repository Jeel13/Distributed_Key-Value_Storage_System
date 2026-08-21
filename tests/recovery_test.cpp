#include <iostream>
#include <cstdio>

#include "kv_store.h"

int main() {

    const std::string walFile = "recovery_test.wal";
    const std::string snapshotFile = "recovery_test.dat";

    std::remove(walFile.c_str());
    std::remove(snapshotFile.c_str());

    {
        KeyValueStore store(walFile, snapshotFile);

        store.put("name", "jeel");
        store.put("city", "Tempe");
        store.put("language", "C++");
        store.put("project", "distributed-kv");

        store.createSnapshot(snapshotFile);

        store.put("status", "active");
        store.put("version", "1");

        store.remove("language");
    }

    std::cout << "PASS: Initial database state created\n";

    {
        KeyValueStore store(walFile, snapshotFile);

        auto name = store.get("name");
        auto city = store.get("city");
        auto language = store.get("language");
        auto project = store.get("project");
        auto status = store.get("status");
        auto version = store.get("version");

        if (!name || *name != "jeel") {
            std::cerr << "FAIL: name incorrect\n";
            return 1;
        }

        if (!city || *city != "Tempe") {
            std::cerr << "FAIL: city incorrect\n";
            return 1;
        }

        if (!project || *project != "distributed-kv") {
            std::cerr << "FAIL: project incorrect\n";
            return 1;
        }

        if (language) {
            std::cerr << "FAIL: language should have been deleted\n";
            return 1;
        }

        if (!status || *status != "active") {
            std::cerr << "FAIL: status incorrect\n";
            return 1;
        }

        if (!version || *version != "1") {
            std::cerr << "FAIL: version incorrect\n";
            return 1;
        }
    }

    std::cout<<"PASS: Recovery reconstructed correct state\n";

    std::remove(walFile.c_str());
    std::remove(snapshotFile.c_str());
    std::remove((walFile + ".tmp").c_str());

    std::cout << "\nAll recovery tests passed.\n";

    return 0;
}