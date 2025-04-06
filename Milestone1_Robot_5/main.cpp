//Alli Demilade
// Adil Farid
// Robot_5
// main.cpp
#include <iostream>
#include "pktDef.h"

int main() {
    // Create a new packet
    PktDef pkt;
    
    // Set packet properties
    pkt.SetPktCount(1);
    pkt.SetCmd(CmdType::DRIVE);  // Set DRIVE command
    
    // Create drive command data
    char driveData[3] = { FORWARD, 5, 85 };  // Direction=FORWARD, Duration=5, Speed=85%
    pkt.SetBodyData(driveData, 3);
    
    // Calculate CRC and generate packet
    pkt.CalcCRC();
    char* rawPacket = pkt.GenPacket();
    
    // Print the packet contents
    std::cout << "Built packet: ";
    for (int i = 0; i < pkt.GetLength(); i++) {
        printf("0x%02X ", (unsigned char)rawPacket[i]);
    }
    std::cout << std::endl;
    
    // Verify CRC
    if (pkt.CheckCRC(rawPacket, pkt.GetLength())) {
        std::cout << "CRC check passed" << std::endl;
    } else {
        std::cout << "CRC check failed" << std::endl;
    }

    return 0;
}
