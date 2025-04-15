//Alli Demilade
// Adil Farid
// Robot_5
// main.cpp
#include "pktDef.h"
#include "MySocket.h"
#include <iostream>
#include <winsock2.h>

int main() {
    try {
        // Step 1: Setup the socket
        MySocket client(SocketType::CLIENT, "127.0.0.1", 5000, ConnectionType::UDP, 1024);
        std::cout << "Connected to robot simulator on port 5000.\n";

        // Step 2: Create and validate Drive command
        PktDef pkt;
        pkt.SetCmd(CmdType::DRIVE);
        pkt.SetAck(true);
        pkt.SetPktCount(1);
        pkt.SetDriveParams(FORWARD, 3, 90); // Explicit values

        // Debug output
        PktDef::DriveBody cmd = pkt.GetDriveParams();
        std::cout << "Command Details:\n"
            << "Direction: " << (int)cmd.Direction << " ("
            << (cmd.Direction == FORWARD ? "FORWARD" : "UNKNOWN") << ")\n"
            << "Duration: " << (int)cmd.Duration << "s\n"
            << "Speed: " << (int)cmd.Speed << "%\n";

        pkt.CalcCRC();
        char* buffer = pkt.GenPacket();

        // Print raw packet
        std::cout << "Raw packet (" << pkt.GetLength() << " bytes): ";
        for (int i = 0; i < pkt.GetLength(); i++) {
            printf("%02X ", (unsigned char)buffer[i]);
        }
        std::cout << "\n";

        // Step 3: Send command
        client.SendData(buffer, pkt.GetLength());
        std::cout << "Command sent.\n";

        // Step 4: Receive response with timeout
        fd_set readSet;
        FD_ZERO(&readSet);
        SOCKET sock = client.GetSocket();
        FD_SET(sock, &readSet);

        timeval timeout;
        timeout.tv_sec = 2;  // 2 second timeout
        timeout.tv_usec = 0;

        char recvBuffer[1024];
        int selectResult = select(0, &readSet, nullptr, nullptr, &timeout);

        if (selectResult == SOCKET_ERROR) {
            throw std::runtime_error("Select failed");
        }
        else if (selectResult == 0) {
            std::cout << "Timeout waiting for response.\n";
            return 1;
        }

        int bytesReceived = client.GetData(recvBuffer);
        std::cout << "Received " << bytesReceived << " bytes.\n";

        if (bytesReceived > 0) {
            std::cout << "Response data: ";
            for (int i = 0; i < bytesReceived; i++) {
                printf("%02X ", (unsigned char)recvBuffer[i]);
            }
            std::cout << "\n";

            try {
                PktDef response(recvBuffer);
                if (response.ValidateCmd() && response.CheckCRC(recvBuffer, bytesReceived)) {
                    if (response.GetAck()) {
                        std::cout << "Valid ACK received.\n";
                    }
                    else {
                        std::cout << "Response received but not an ACK.\n";
                    }
                }
                else {
                    std::cout << "Invalid response format.\n";
                }
            }
            catch (...) {
                std::cout << "Malformed response packet.\n";
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}