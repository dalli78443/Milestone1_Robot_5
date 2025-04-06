//Alli Demilade
// Adil Farid
// Robot_5
// PktDef.h
#pragma once
#include <vector>

class PktDef {
public:
    enum CommandFlag {
        DRIVE = 0x01,
        STATUS = 0x02,
        SLEEP = 0x04,
        ACK = 0x08
    };

    enum Direction {
        FORWARD = 1,
        BACKWARD = 2,
        RIGHT = 3,
        LEFT = 4
    };

    // Constructor
    PktDef();

    // Setters
    void SetPktCount(unsigned short count);
    void SetCmd(bool drive, bool status, bool sleep, bool ack);
    void SetDriveData(unsigned char direction, unsigned char duration, unsigned char speed);
    void BuildPacket();

    // Getters
    unsigned short GetPktCount() const;
    unsigned char GetCmdFlags() const;
    unsigned short GetLength() const;
    std::vector<unsigned char> GetData() const;
    unsigned char GetCRC() const;
    std::vector<unsigned char> GetPacket() const;

    

private:
    // Data fields
    unsigned short PktCount;
    unsigned char CmdFlags;
    unsigned short Length;
    std::vector<unsigned char> Data;
    unsigned char CRC;

    std::vector<unsigned char> PacketBytes;
    // CRC utility
    unsigned char CalcCRC(const std::vector<unsigned char>& data);
    // Internal helpers
    void ClearData();
};
