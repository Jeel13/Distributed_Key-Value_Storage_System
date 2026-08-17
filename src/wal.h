#ifndef WAL_H
#define WAL_H

#include <fstream>
#include <mutex>
#include <string>
#include <vector>

class WAL {
private:
    std::ofstream file;
    std::mutex mutex;
    std::string filename;

public:
    explicit WAL(const std::string& filename);

    void appendPut(
        const std::string& key,
        const std::string& value
    );

    void appendDelete(
        const std::string& key
    );

    std::vector<std::string> readAll();
};

#endif