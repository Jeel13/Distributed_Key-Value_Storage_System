#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <atomic>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8080;

bool sendRequest(
    const std::string& request,
    std::string& response
) {
    int socketFd= socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if(socketFd == -1){
        return false;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family=AF_INET;
    serverAddress.sin_port=htons(SERVER_PORT);

    if(inet_pton(
            AF_INET,
            SERVER_IP,
            &serverAddress.sin_addr
        )<= 0) {

        close(socketFd);
        return false;
    }

    if(connect(
        socketFd,
        reinterpret_cast<sockaddr*>(
            &serverAddress
        ),
        sizeof(serverAddress)
    ) == -1){
        close(socketFd);
        return false;
    }

    std::string message=request+ "\n";

    ssize_t sent=send(
        socketFd,
        message.data(),
        message.size(),
        0
    );

    if(sent != static_cast<ssize_t>(message.size())){
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

    if(bytesRead <= 0) {
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
    const int requestsPerClient = 100;

    std::atomic<int> successfulRequests{0};
    std::atomic<int> failedRequests{0};

    std::vector<std::thread> threads;

    auto start =std::chrono::steady_clock::now();

    for(int i=0; i<clientCount; i++) {

        threads.emplace_back(
            [i, requestsPerClient, &successfulRequests, &failedRequests]() {
                for (int j=0; j<requestsPerClient; j++) {

                    std::string key ="bench_" + std::to_string(i)+"_" +std::to_string(j);

                    std::string value ="value_" +std::to_string(j);

                    std::string response;

                    if (!sendRequest(
                            "PUT " +
                            key +
                            " " +
                            value,
                            response
                        )) {
                        failedRequests++;
                        continue;
                    }

                    if (response != "OK\n") {
                        failedRequests++;
                        continue;
                    }

                    successfulRequests++;

                    if (!sendRequest(
                            "GET " + key,
                            response
                        )){
                        failedRequests++;
                        continue;
                    }

                    std::string expected ="VALUE "+value+"\n";

                    if(response != expected){
                        failedRequests++;
                        continue;
                    }

                    successfulRequests++;
                }
            }
        );
    }

    for(auto& thread : threads){
        thread.join();
    }

    auto end=std::chrono::steady_clock::now();

    auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    int totalRequests= clientCount * requestsPerClient * 2;

    double seconds= duration.count() / 1000.0;

    double throughput= totalRequests / seconds;

    std::cout << "\n========== Benchmark ==========\n";

    std::cout<< "Clients: "<< clientCount<< "\n";

    std::cout<< "Requests per client: "<< requestsPerClient<< "\n";

    std::cout<< "Total requests: "<< totalRequests<< "\n";

    std::cout<< "Successful requests: "<< successfulRequests.load()<< "\n";

    std::cout<< "Failed requests: "<< failedRequests.load()<< "\n";

    std::cout<< "Total time: "<< seconds<< " seconds\n";

    std::cout<< "Throughput: "<< throughput<< " requests/sec\n";

    std::cout<< "===============================\n";

    if(failedRequests==0){
        std::cout<< "PASS: Benchmark completed successfully\n";

        return 0;
    }

    std::cout<< "FAIL: Some requests failed\n";

    return 1;
}