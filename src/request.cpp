#include "request.h"
#include <sstream>

std::optional<Request> parseRequest(const std::string& input){
    std::istringstream stream(input);

    std::string command;
    std::string key;
    std::string value;

    stream>>command>>key;

    if(command=="GET"){
        if(key.empty()){
            return std::nullopt;
        }

        return Request{
            Command::GET,
            key,
            ""
        };
    }

    if(command=="PUT"){
        stream>>value;

        if(key.empty() || value.empty()){
            return std::nullopt;
        }

        return Request{
            Command::PUT,
            key,
            value
        };
    }

    if(command=="DELETE"){
        if(key.empty()){
            return std::nullopt;
        }

        return Request{
            Command::DELETE,
            key,
            ""
        };
    }

    return std::nullopt;
}
