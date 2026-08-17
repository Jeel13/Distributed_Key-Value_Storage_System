#include "client_handler.h"

#include <iostream>
#include <string>

#include <sys/socket.h>
#include <unistd.h>

#include "handler.h"

void handleClient(
    int clientSocket,
    KeyValueStore& store
) {
    std::cout << "Client connected\n";

    std::string buffer;
    char temp[1024];

    while(true) {

        ssize_t bytesRead = recv(
            clientSocket,
            temp,
            sizeof(temp),
            0
        );

        if(bytesRead<=0){
            break;
        }

        buffer.append(temp, bytesRead);

        size_t newlinePosition;

        while (
            (newlinePosition=buffer.find('\n')) != std::string::npos
        ) {
            std::string request = buffer.substr(0, newlinePosition);

            buffer.erase(
                0,
                newlinePosition + 1
            );

            handleRequest(
                request,
                store,
                clientSocket
            );
        }
    }

    std::cout<<"Client disconnected\n";

    close(clientSocket);
}