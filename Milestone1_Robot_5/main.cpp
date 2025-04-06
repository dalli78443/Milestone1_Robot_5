// main.cpp
#include <iostream>
#include "pktDef.h"

int main() {
    PktDef pkt;
    pkt.SetPktCount(1);
    pkt.SetCmd(true, false, false, false); // Set DRIVE only
    pkt.SetDriveData(0x01, 0x05, 0x64);    // Direction=1, Duration=5, Speed=100
    pkt.BuildPacket();

    auto packet = pkt.GetPacket();
    std::cout << "Built packet: ";
    for (auto b : packet) {
        printf("0x%02X ", b);
    }
    std::cout << std::endl;

    return 0;
}
