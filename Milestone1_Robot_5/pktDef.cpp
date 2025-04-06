//Alli Demilade
// Adil Farid
// Robot_5
// PktDef.cpp
#include "pktDef.h"
#include <cstring>

// Default constructor
PktDef::PktDef() {
    packet.header.PktCount = 0;
    packet.header.Command.Drive = 0;
    packet.header.Command.Status = 0;
    packet.header.Command.Sleep = 0;
    packet.header.Command.Ack = 0;
    packet.header.Command.Padding = 0;
    packet.header.Length = 0;
    packet.Data = nullptr;
    packet.CRC = 0;
    RawBuffer = nullptr;
}

// Constructor with raw data
PktDef::PktDef(char* rawData) {
    // Copy header
    memcpy(&packet.header, rawData, HEADERSIZE);

    // Allocate and copy body data if length > 0
    if (packet.header.Length > HEADERSIZE + 1) { // +1 for CRC
        int bodySize = packet.header.Length - HEADERSIZE - 1;
        packet.Data = new char[bodySize];
        memcpy(packet.Data, rawData + HEADERSIZE, bodySize);
    }
    else {
        packet.Data = nullptr;
    }

    // Copy CRC
    packet.CRC = rawData[packet.header.Length - 1];
    RawBuffer = nullptr;
}

// Destructor
PktDef::~PktDef() {
    delete[] packet.Data;
    delete[] RawBuffer;
}

void PktDef::SetCmd(CmdType cmd) {
    packet.header.Command.Drive = 0;
    packet.header.Command.Status = 0;
    packet.header.Command.Sleep = 0;

    // Clear any existing body data for SLEEP and RESPONSE commands
    if (cmd == CmdType::SLEEP || cmd == CmdType::RESPONSE) {
        delete[] packet.Data;
        packet.Data = nullptr;
        packet.header.Length = HEADERSIZE + 1;  // Header + CRC only
    }

    switch (cmd) {
    case CmdType::DRIVE:
        packet.header.Command.Drive = 1;
        break;
    case CmdType::SLEEP:
        packet.header.Command.Sleep = 1;
        break;
    case CmdType::RESPONSE:
        packet.header.Command.Status = 1;
        break;
    }
}

void PktDef::SetBodyData(char* data, int size) {
    delete[] packet.Data;
    packet.Data = new char[size];
    memcpy(packet.Data, data, size);
    packet.header.Length = HEADERSIZE + size + 1; // +1 for CRC
}

void PktDef::SetPktCount(int count) {
    packet.header.PktCount = count;
}

CmdType PktDef::GetCmd() {
    if (packet.header.Command.Drive) return CmdType::DRIVE;
    if (packet.header.Command.Sleep) return CmdType::SLEEP;
    if (packet.header.Command.Status) return CmdType::RESPONSE;
    return CmdType::DRIVE; // Default case
}

bool PktDef::GetAck() {
    return packet.header.Command.Ack == 1;
}

int PktDef::GetLength() {
    return packet.header.Length;
}

char* PktDef::GetBodyData() {
    return packet.Data;
}

int PktDef::GetPktCount() {
    return packet.header.PktCount;
}

bool PktDef::CheckCRC(char* data, int size) {
    char calculatedCRC = 0;
    for (int i = 0; i < size - 1; i++) { // -1 to exclude CRC byte
        for (int bit = 0; bit < 8; bit++) {
            if (data[i] & (1 << bit)) {
                calculatedCRC++;
            }
        }
    }
    return calculatedCRC == data[size - 1];
}

void PktDef::CalcCRC() {
    char calculatedCRC = 0;

    // Count bits in header
    unsigned char* headerBytes = (unsigned char*)&packet.header;
    for (int i = 0; i < HEADERSIZE; i++) {
        for (int bit = 0; bit < 8; bit++) {
            if (headerBytes[i] & (1 << bit)) {
                calculatedCRC++;
            }
        }
    }

    // Count bits in body if exists
    if (packet.Data != nullptr) {
        int bodySize = packet.header.Length - HEADERSIZE - 1;
        for (int i = 0; i < bodySize; i++) {
            for (int bit = 0; bit < 8; bit++) {
                if (packet.Data[i] & (1 << bit)) {
                    calculatedCRC++;
                }
            }
        }
    }

    packet.CRC = calculatedCRC;
}

char* PktDef::GenPacket() {
    delete[] RawBuffer;
    RawBuffer = new char[packet.header.Length];

    // Copy header
    memcpy(RawBuffer, &packet.header, HEADERSIZE);

    // Copy body if exists
    if (packet.Data != nullptr) {
        int bodySize = packet.header.Length - HEADERSIZE - 1;
        memcpy(RawBuffer + HEADERSIZE, packet.Data, bodySize);
    }

    // Copy CRC
    RawBuffer[packet.header.Length - 1] = packet.CRC;

    return RawBuffer;
}