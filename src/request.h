#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <optional>

enum class Command {
    GET,
    PUT,
    DELETE,
    INVALID
};

struct Request {
    Command command;
    std::string key;
    std::string value;
};

std::optional<Request> parseRequest(const std::string& input);

#endif