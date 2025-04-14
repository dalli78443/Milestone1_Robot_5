//Alli Demilade
// Adil Farid
// Robot_5
// main.cpp
#include "MySocket.h"
#include <iostream>
#include <string>
#include <cstring>

void printUsage() {
    std::cout << "Usage:\n"
              << "  To run as server: program -s [port]\n"
              << "  To run as client: program -c [server_ip] [port]\n"
              << "Example:\n"
              << "  Server: program -s 27015\n"
              << "  Client: program -c 192.168.1.100 27015\n";
}

void runServer(int port) {
    try {
        MySocket server(SocketType::SERVER, "0.0.0.0", port, ConnectionType::TCP);
        std::cout << "Server initialized on port " << port << ". Waiting for connections..." << std::endl;

        server.ConnectTCP();
        std::cout << "Client connected!" << std::endl;

        char buffer[1024];
        while (true) {
            int bytesReceived = server.GetData(buffer);
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';
                std::cout << "Received: " << buffer << std::endl;
                
                // Echo back
                server.SendData(buffer, bytesReceived);
                std::cout << "Echoed back" << std::endl;
            }
            else if (bytesReceived == 0) {
                std::cout << "Client disconnected" << std::endl;
                break;
            }
        }

        server.DisconnectTCP();
    }
    catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
}

void runClient(const std::string& serverIP, int port) {
    try {
        MySocket client(SocketType::CLIENT, serverIP, port, ConnectionType::TCP);
        std::cout << "Client initialized. Connecting to " << serverIP << ":" << port << "..." << std::endl;

        client.ConnectTCP();
        std::cout << "Connected to server!" << std::endl;

        std::string input;
        char buffer[1024];

        while (true) {
            std::cout << "Enter message (or 'quit' to exit): ";
            std::getline(std::cin, input);

            if (input == "quit") {
                break;
            }

            client.SendData(input.c_str(), input.length() + 1);
            std::cout << "Sent: " << input << std::endl;

            int bytesReceived = client.GetData(buffer);
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';
                std::cout << "Received: " << buffer << std::endl;
            }
        }

        client.DisconnectTCP();
    }
    catch (const std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage();
        return 1;
    }

    if (strcmp(argv[1], "-s") == 0) {
        // Server mode
        int port = (argc > 2) ? std::stoi(argv[2]) : 27015;
        runServer(port);
    }
    else if (strcmp(argv[1], "-c") == 0 && argc >= 4) {
        // Client mode
        std::string serverIP = argv[2];
        int port = std::stoi(argv[3]);
        runClient(serverIP, port);
    }
    else {
        printUsage();
        return 1;
    }

    return 0;
}
