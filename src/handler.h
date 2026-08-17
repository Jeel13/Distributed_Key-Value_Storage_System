#ifndef HANDLER_H
#define HANDLER_H

#include <string>

#include "kv_store.h"

void handleRequest(
    const std::string& input,
    KeyValueStore& store,
    int clientSocket
);

#endif

