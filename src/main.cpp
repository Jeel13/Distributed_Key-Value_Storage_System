#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "kv_store.h"
#include "thread_pool.h"
#include "client_handler.h"

int main(){
    //key value store
    KeyValueStore store("data.wal", "snapshot.dat");

    ThreadPool pool(4);

    // Create socket
    int serverSocket = socket(
        AF_INET, 
        SOCK_STREAM, 
        0
    );

    if(serverSocket == -1){
        std::cerr<<"Failed to create socket\n";
        return 1;
    }

    std::cout<<"Socket created\n";

    // Configure server address
    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    // Bind socket to port
    if(bind(
        serverSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    ) == -1){
        std::cout<<"Failed\n";
        std::cerr<<"Failed to bind socket\n";
        close(serverSocket);
        return 1;
    }

    std::cout<<"Server bound to port 8080\n";

    // Start listening
    if(listen(serverSocket, 10)==-1){
        std::cerr<<"Failed to listen\n";
        close(serverSocket);
        return 1;
    }

    std::cout<<"Server Listening...\n";

    while(true){

        // Accept a client
        sockaddr_in clientAddress{};
        socklen_t clientSize = sizeof(clientAddress);
        
        int clientSocket=accept(
            serverSocket,
            reinterpret_cast<sockaddr*>(&clientAddress),
            &clientSize
        );
    
        if(clientSocket==-1){
            std::cerr<<"Failed to accept client\n";
            continue;
        }

        pool.enqueue(
            [clientSocket, &store](){
                handleClient(
                    clientSocket,
                    store
                );
            }
        );
    }


    close(serverSocket);

    return 0;
}


//test store
// int main() {
//     KeyValueStore store;

//     store.put("name", "jeel");
//     store.put("language", "cpp");

//     auto name=store.get("name");

//     if(name){
//         std::cout<<"name = "<< *name <<"\n";
//     }

//     auto age = store.get("age");

//     if(!age){
//         std::cout<<"age not found\n";
//     }

//     store.remove("language");

//     auto language = store.get("language");

//     if(!language) {
//         std::cout<<"language removed\n";
//     }

//     return 0;
// }


//test request
// #include "request.h"

// int main() {
//     auto request=parseRequest("PUT name jeel");

//     if(!request){
//         std::cout<<"Invalid request\n";
//         return 0;
//     }

//     if(request->command==Command::PUT){
//         std::cout<<"Command: PUT\n";
//         std::cout<<"Key: "<<request->key<<"\n";
//         std::cout<<"Value: "<<request->value<<"\n";
//     }
    
//     return 0;
// }


//test threadpool
// #include <chrono>
// #include <thread>

// #include "thread_pool.h"

// int main() {
//     ThreadPool pool(4);

//     for(int i=0; i<10; i++){
//         pool.enqueue([i](){
//             std::cout
//                 <<"Task "
//                 << i
//                 <<" running on thread "
//                 << std::this_thread::get_id()
//                 <<"\n";

//             std::this_thread::sleep_for(
//                 std::chrono::seconds(1)
//             );
//         });
//     }

//     std::this_thread::sleep_for(
//         std::chrono::seconds(3)
//     );

//     return 0;
// }