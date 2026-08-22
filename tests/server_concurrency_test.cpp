#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8080;

bool sendRequest(
    const std::string& request,
    std::string& response
) {
    int socketFd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (socketFd == -1) {
        return false;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port =
        htons(SERVER_PORT);

    if (inet_pton(
            AF_INET,
            SERVER_IP,
            &serverAddress.sin_addr
        ) <= 0) {

        close(socketFd);
        return false;
    }

    if (connect(
            socketFd,
            reinterpret_cast<sockaddr*>(
                &serverAddress
            ),
            sizeof(serverAddress)
        ) == -1) {

        close(socketFd);
        return false;
    }

    std::string message =
        request + "\n";

    ssize_t sent = send(
        socketFd,
        message.data(),
        message.size(),
        0
    );

    if (sent != static_cast<ssize_t>(
            message.size())) {

        close(socketFd);
        return false;
    }

    char buffer[1024];

    ssize_t bytesRead = recv(
        socketFd,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if (bytesRead <= 0) {

        close(socketFd);
        return false;
    }

    buffer[bytesRead] = '\0';

    response = buffer;

    close(socketFd);

    return true;
}


int main() {

    const int clientCount = 10;

    std::vector<std::thread> threads;

    std::vector<bool> results(
        clientCount,
        false
    );

    // --------------------------------------------------
    // Concurrent clients
    // --------------------------------------------------

    for (int i = 0; i < clientCount; i++) {

        threads.emplace_back(
            [i, &results]() {

                std::string key =
                    "client_key_" +
                    std::to_string(i);

                std::string value =
                    "client_value_" +
                    std::to_string(i);

                // PUT
                std::string response;

                if (!sendRequest(
                        "PUT " + key + " " + value,
                        response
                    )) {

                    return;
                }

                if (response != "OK\n") {
                    return;
                }

                // GET
                if (!sendRequest(
                        "GET " + key,
                        response
                    )) {

                    return;
                }

                std::string expected =
                    "VALUE " + value + "\n";

                if (response != expected) {
                    return;
                }

                results[i] = true;
            }
        );
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // --------------------------------------------------
    // Verify results
    // --------------------------------------------------

    for (int i = 0; i < clientCount; i++) {

        if (!results[i]) {

            std::cerr
                << "FAIL: Client "
                << i
                << " failed\n";

            return 1;
        }
    }

    std::cout
        << "PASS: "
        << clientCount
        << " concurrent clients completed\n";

    // --------------------------------------------------
    // DELETE test
    // --------------------------------------------------

    threads.clear();

    results.assign(
        clientCount,
        false
    );

    for (int i = 0; i < clientCount; i++) {

        threads.emplace_back(
            [i, &results]() {

                std::string key =
                    "client_key_" +
                    std::to_string(i);

                std::string response;

                if (!sendRequest(
                        "DELETE " + key,
                        response
                    )) {

                    return;
                }

                if (response != "DELETED\n") {
                    return;
                }

                if (!sendRequest(
                        "GET " + key,
                        response
                    )) {

                    return;
                }

                if (response != "NOT_FOUND\n") {
                    return;
                }

                results[i] = true;
            }
        );
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (int i = 0; i < clientCount; i++) {

        if (!results[i]) {

            std::cerr
                << "FAIL: DELETE test failed for client "
                << i
                << "\n";

            return 1;
        }
    }

    std::cout
        << "PASS: Concurrent DELETE operations verified\n";

    std::cout
        << "\nAll server concurrency tests passed.\n";

    return 0;
}