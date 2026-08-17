#include <cstring>

#include <sys/socket.h>

#include "handler.h"
#include "request.h"

void handleRequest(
    const std::string& input,
    KeyValueStore& store,
    int clientSocket
) {
    auto request=parseRequest(input);

    if(!request){
        const char* response = "ERROR invalid_request\n";

        send(
            clientSocket,
            response,
            strlen(response),
            0
        );

        return;
    }

    if(request->command==Command::PUT){
        store.put(
            request->key,
            request->value
        );

        const char* response="OK\n";

        send(
            clientSocket,
            response,
            strlen(response),
            0
        );

        return;
    }

    if(request->command==Command::GET){
        auto value=store.get(request->key);

        if(value){
            std::string response = "VALUE "+*value+"\n";

            send(
                clientSocket,
                response.c_str(),
                response.size(),
                0
            );
        } else {
            const char* response="NOT_FOUND\n";

            send(
                clientSocket,
                response,
                strlen(response),
                0
            );
        }

        return;
    }

    if(request->command==Command::DELETE){
        bool removed=store.remove(request->key);

        const char* response= removed? "DELETED\n" : "NOT_FOUND\n";

        send(
            clientSocket,
            response,
            strlen(response),
            0
        );

        return;
    }
}