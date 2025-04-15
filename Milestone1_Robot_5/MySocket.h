#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

// Socket type enumeration
enum class SocketType {
    CLIENT,  // Client socket type
    SERVER   // Server socket type
};

// Connection type enumeration
enum class ConnectionType {
    TCP,     // Connection-oriented protocol
    UDP      // Connectionless protocol
};

// Default buffer size for socket communication
const int DEFAULT_SIZE = 1024;


class MySocket {
private:
    char* Buffer;                    // Dynamic buffer for communication
    SOCKET WelcomeSocket;            // Server welcome socket
    SOCKET ConnectionSocket;         // Connection socket for both TCP and UDP
    sockaddr_in SvrAddr;             // Server address structure
    SocketType mySocket;             // Socket type (CLIENT/SERVER)
    std::string IPAddr;              // IPv4 address
    int Port;                        // Port number
    ConnectionType connectionType;    // Transport protocol (TCP/UDP)
    bool bTCPConnect;                // TCP connection status
    int MaxSize;                     // Maximum buffer size

public:
   
    MySocket(SocketType socketType, std::string ipAddress, unsigned int port, 
             ConnectionType connType, unsigned int bufferSize = DEFAULT_SIZE);

    ~MySocket();

    void ConnectTCP();

    void DisconnectTCP();

    void SendData(const char* data, int size);


    int GetData(char* data);


    std::string GetIPAddr() const;


    void SetIPAddr(std::string ipAddress);

    void SetPort(int port);


    int GetPort() const;

    SocketType GetType() const;
    void SetType(SocketType type);
    // In MySocket.h public section:
    SOCKET GetSocket() const {
        return ConnectionSocket;
    }

private:

    void InitializeWinsock();


    void SetupSocket();

    void SetupAddress();

    void ValidateBufferSize(unsigned int& size);
};
