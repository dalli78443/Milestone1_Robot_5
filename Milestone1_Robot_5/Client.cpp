#include "MySocket.h"
#include <iostream>
#include <string>

int main() {
    try {
        // Initialize client socket
        MySocket client(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP);
        std::cout << "Client initialized. Connecting to server..." << std::endl;

        // Connect to server
        client.ConnectTCP();
        std::cout << "Connected to server!" << std::endl;

        std::string input;
        char buffer[1024];

        while (true) {
            // Get input from user
            std::cout << "Enter message (or 'quit' to exit): ";
            std::getline(std::cin, input);

            if (input == "quit") {
                break;
            }

            // Send data to server
            client.SendData(input.c_str(), input.length() + 1);
            std::cout << "Sent to server: " << input << std::endl;

            // Receive echo from server
            int bytesReceived = client.GetData(buffer);
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0'; // Null terminate the string
                std::cout << "Received from server: " << buffer << std::endl;
            }
        }

        // Cleanup
        client.DisconnectTCP();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 