#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "kv_store.h"

void handleClient(
    int clientSocket,
    KeyValueStore& store
);

#endif