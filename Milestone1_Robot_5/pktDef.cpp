// PktDef.cpp
#include "pktDef.h"

// Constructor
PktDef::PktDef()
    : PktCount(0), CmdFlags(0), Length(0), CRC(0) {
    ClearData();
}

// Set the packet count
void PktDef::SetPktCount(unsigned short count) {
    PktCount = count;
}

// Set the command flags (bit-wise logic)
void PktDef::SetCmd(bool drive, bool status, bool sleep, bool ack) {
    CmdFlags = 0;
    if (drive)  CmdFlags |= DRIVE;
    if (status) CmdFlags |= STATUS;
    if (sleep)  CmdFlags |= SLEEP;
    if (ack)    CmdFlags |= ACK;
}

// Helper to reset data
void PktDef::ClearData() {
    Data.clear();
    Length = 0;
    CRC = 0;
    PacketBytes.clear();
}
// Set drive parameters (for Drive commands only)
void PktDef::SetDriveData(unsigned char direction, unsigned char duration, unsigned char speed) {
    if (!(CmdFlags & DRIVE)) return; // Only set if Drive flag is active

    Data.clear();
    Data.push_back(direction);
    Data.push_back(duration);
    Data.push_back(speed);

    Length = static_cast<unsigned short>(Data.size() + 6); // 6 bytes = header(4) + length(1) + CRC(1)
}

// Build packet byte-by-byte
void PktDef::BuildPacket() {
    PacketBytes.clear();

    // Header
    PacketBytes.push_back(PktCount & 0xFF);          // Low byte
    PacketBytes.push_back((PktCount >> 8) & 0xFF);   // High byte
    PacketBytes.push_back(CmdFlags);                 // CmdFlags
    PacketBytes.push_back(static_cast<unsigned char>(Length)); // Length

    // Body
    for (auto b : Data) {
        PacketBytes.push_back(b);
    }

    // CRC
    CRC = CalcCRC(PacketBytes); // Only on everything before CRC
    PacketBytes.push_back(CRC); // Append to end
}
unsigned char PktDef::CalcCRC(const std::vector<unsigned char>& data) {
    unsigned char crc = 0;
    for (unsigned char byte : data) {
        for (int i = 0; i < 8; ++i) {
            if (byte & (1 << i)) crc++;
        }
    }
    return crc;
}


unsigned short PktDef::GetPktCount() const {
    return PktCount;
}

unsigned char PktDef::GetCmdFlags() const {
    return CmdFlags;
}

unsigned short PktDef::GetLength() const {
    return Length;
}

std::vector<unsigned char> PktDef::GetData() const {
    return Data;
}

unsigned char PktDef::GetCRC() const {
    return CRC;
}

std::vector<unsigned char> PktDef::GetPacket() const {
    return PacketBytes;
}
