#include "MySocket.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

MySocket::MySocket(SocketType socketType, std::string ipAddress, unsigned int port,
    ConnectionType connType, unsigned int bufferSize) {
    // Initialize member variables
    mySocket = socketType;
    IPAddr = ipAddress;
    Port = port;
    connectionType = connType;
    bTCPConnect = false;
    WelcomeSocket = INVALID_SOCKET;
    ConnectionSocket = INVALID_SOCKET;

    // Validate and set buffer size
    ValidateBufferSize(bufferSize);
    MaxSize = bufferSize;
    Buffer = new char[MaxSize];

    // Initialize Winsock
    InitializeWinsock();

    // Setup socket and address
    SetupSocket();
    SetupAddress();
}

MySocket::~MySocket() {
    // Cleanup
    if (bTCPConnect) {
        DisconnectTCP();
    }
    if (WelcomeSocket != INVALID_SOCKET) {
        closesocket(WelcomeSocket);
    }
    if (ConnectionSocket != INVALID_SOCKET) {
        closesocket(ConnectionSocket);
    }
    delete[] Buffer;
    WSACleanup();
}

void MySocket::ConnectTCP() {
    if (connectionType != ConnectionType::TCP) {
        std::stringstream ss;
        ss << "Cannot establish TCP connection on UDP socket (IP: " << IPAddr << ", Port: " << Port << ")";
        throw std::runtime_error(ss.str());
    }

    if (mySocket == SocketType::SERVER) {
        // Bind the welcome socket
        if (bind(WelcomeSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
            std::stringstream ss;
            ss << "Failed to bind socket (IP: " << IPAddr << ", Port: " << Port << ")";
            throw std::runtime_error(ss.str());
        }

        // Listen for connections
        if (listen(WelcomeSocket, 1) == SOCKET_ERROR) {
            std::stringstream ss;
            ss << "Failed to listen on socket (IP: " << IPAddr << ", Port: " << Port << ")";
            throw std::runtime_error(ss.str());
        }

        // Accept connection
        ConnectionSocket = accept(WelcomeSocket, NULL, NULL);
        if (ConnectionSocket == INVALID_SOCKET) {
            std::stringstream ss;
            ss << "Failed to accept connection (IP: " << IPAddr << ", Port: " << Port << ")";
            throw std::runtime_error(ss.str());
        }
    }
    else {
        // Connect to server
        if (connect(ConnectionSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
            std::stringstream ss;
            ss << "Failed to connect to server (IP: " << IPAddr << ", Port: " << Port << ")";
            throw std::runtime_error(ss.str());
        }
    }

    bTCPConnect = true;
}

void MySocket::DisconnectTCP() {
    if (!bTCPConnect) {
        return;
    }

    if (mySocket == SocketType::SERVER) {
        closesocket(ConnectionSocket);
        ConnectionSocket = INVALID_SOCKET;
    }
    else {
        shutdown(ConnectionSocket, SD_BOTH);
        closesocket(ConnectionSocket);
        ConnectionSocket = INVALID_SOCKET;
    }

    bTCPConnect = false;
}

void MySocket::SendData(const char* data, int size) {
    if (size > MaxSize) {
        std::stringstream ss;
        ss << "Data size (" << size << " bytes) exceeds buffer capacity (" << MaxSize << " bytes)";
        throw std::runtime_error(ss.str());
    }

    int bytesSent;
    if (connectionType == ConnectionType::TCP) {
        if (!bTCPConnect) {
            throw std::runtime_error("Cannot send data: TCP connection not established");
        }
        bytesSent = send(ConnectionSocket, data, size, 0);
    }
    else {
        bytesSent = sendto(ConnectionSocket, data, size, 0,
            (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));
    }

    if (bytesSent == SOCKET_ERROR) {
        std::stringstream ss;
        ss << "Failed to send data (IP: " << IPAddr << ", Port: " << Port << ")";
        throw std::runtime_error(ss.str());
    }
}

int MySocket::GetData(char* data) {
    int bytesReceived;
    if (connectionType == ConnectionType::TCP) {
        if (!bTCPConnect) {
            throw std::runtime_error("Cannot receive data: TCP connection not established");
        }
        bytesReceived = recv(ConnectionSocket, Buffer, MaxSize, 0);
    }
    else {
        int addrLen = sizeof(SvrAddr);
        bytesReceived = recvfrom(ConnectionSocket, Buffer, MaxSize, 0,
            (struct sockaddr*)&SvrAddr, &addrLen);
    }

    if (bytesReceived == SOCKET_ERROR) {
        std::stringstream ss;
        ss << "Failed to receive data (IP: " << IPAddr << ", Port: " << Port << ")";
        throw std::runtime_error(ss.str());
    }

    // Copy received data to provided buffer
    memcpy(data, Buffer, bytesReceived);
    return bytesReceived;
}

// Getters and Setters
std::string MySocket::GetIPAddr() const {
    return IPAddr;
}

void MySocket::SetIPAddr(std::string ipAddress) {
    if (bTCPConnect || WelcomeSocket != INVALID_SOCKET) {
        std::stringstream ss;
        ss << "Cannot change IP address while socket is connected (Current IP: " << IPAddr << ")";
        throw std::runtime_error(ss.str());
    }
    IPAddr = ipAddress;
    SetupAddress();
}

void MySocket::SetPort(int port) {
    if (bTCPConnect || WelcomeSocket != INVALID_SOCKET) {
        std::stringstream ss;
        ss << "Cannot change port while socket is connected (Current Port: " << Port << ")";
        throw std::runtime_error(ss.str());
    }
    Port = port;
    SetupAddress();
}

int MySocket::GetPort() const {
    return Port;
}

SocketType MySocket::GetType() const {
    return mySocket;
}

void MySocket::SetType(SocketType type) {
    if (bTCPConnect || WelcomeSocket != INVALID_SOCKET) {
        std::stringstream ss;
        ss << "Cannot change socket type while socket is connected (Current Type: " 
           << (mySocket == SocketType::CLIENT ? "CLIENT" : "SERVER") << ")";
        throw std::runtime_error(ss.str());
    }
    mySocket = type;
    SetupSocket();
}

// Helper functions
void MySocket::InitializeWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("Failed to initialize Winsock: WSAStartup failed");
    }
}

void MySocket::SetupSocket() {
    // Close existing sockets
    if (WelcomeSocket != INVALID_SOCKET) {
        closesocket(WelcomeSocket);
    }
    if (ConnectionSocket != INVALID_SOCKET) {
        closesocket(ConnectionSocket); 
    }

    // Create appropriate socket
    int type = (connectionType == ConnectionType::TCP) ? SOCK_STREAM : SOCK_DGRAM;
    int protocol = (connectionType == ConnectionType::TCP) ? IPPROTO_TCP : IPPROTO_UDP;

    if (mySocket == SocketType::SERVER && connectionType == ConnectionType::TCP) {
        WelcomeSocket = socket(AF_INET, type, protocol);
        if (WelcomeSocket == INVALID_SOCKET) {
            std::stringstream ss;
            ss << "Failed to create welcome socket (IP: " << IPAddr << ", Port: " << Port << ")";
            throw std::runtime_error(ss.str());
        }
    }

    ConnectionSocket = socket(AF_INET, type, protocol);
    if (ConnectionSocket == INVALID_SOCKET) {
        std::stringstream ss;
        ss << "Failed to create connection socket (IP: " << IPAddr << ", Port: " << Port << ")";
        throw std::runtime_error(ss.str());
    }
}

void MySocket::SetupAddress() {
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(Port);
    inet_pton(AF_INET, IPAddr.c_str(), &SvrAddr.sin_addr);
}

void MySocket::ValidateBufferSize(unsigned int& size) {
    if (size <= 0) {
        size = DEFAULT_SIZE;
    }
}

