#include "MySocket.h"
#include <iostream>
#include <string>

int main() {
    try {
        // Initialize server socket
        MySocket server(SocketType::SERVER, "127.0.0.1", 27015, ConnectionType::TCP);
        std::cout << "Server initialized. Waiting for connections..." << std::endl;

        // Connect and wait for client
        server.ConnectTCP();
        std::cout << "Client connected!" << std::endl;

        char buffer[1024];
        while (true) {
            // Receive data from client
            int bytesReceived = server.GetData(buffer);
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0'; // Null terminate the string
                std::cout << "Received from client: " << buffer << std::endl;

                // Echo back to client
                server.SendData(buffer, bytesReceived);
                std::cout << "Echoed back to client" << std::endl;
            }
            else if (bytesReceived == 0) {
                std::cout << "Client disconnected" << std::endl;
                break;
            }
        }

        // Cleanup
        server.DisconnectTCP();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 