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
    // Clear all command flags first
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

void PktDef::SetAck(bool isAck) {
    packet.header.Command.Ack = isAck ? 1 : 0;
}

void PktDef::SetTelemetryData(unsigned short lastPktCounter, unsigned short currentGrade,
                             unsigned short hitCount, unsigned char lastCmd,
                             unsigned char lastCmdValue, unsigned char lastCmdSpeed) {
    // Create and fill telemetry structure
    TelemetryBody telemetry;
    telemetry.LastPktCounter = lastPktCounter;
    telemetry.CurrentGrade = currentGrade;
    telemetry.HitCount = hitCount;
    telemetry.LastCmd = lastCmd;
    telemetry.LastCmdValue = lastCmdValue;
    telemetry.LastCmdSpeed = lastCmdSpeed;

    // Set command type to RESPONSE
    SetCmd(CmdType::RESPONSE);

    // Copy telemetry data to packet body
    delete[] packet.Data;
    packet.Data = new char[sizeof(TelemetryBody)];
    memcpy(packet.Data, &telemetry, sizeof(TelemetryBody));
    packet.header.Length = HEADERSIZE + sizeof(TelemetryBody) + 1; // +1 for CRC
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

void PktDef::SetDriveParams(unsigned char direction, unsigned char duration, unsigned char speed) {
    // Validate direction
    if (direction != FORWARD && direction != BACKWARD && 
        direction != RIGHT && direction != LEFT) {
        return; // Invalid direction
    }

    // Validate speed (must be between 80-100%)
    if (speed < 80 || speed > 100) {
        return; // Invalid speed
    }

    // Set command type to DRIVE
    SetCmd(CmdType::DRIVE);

    // Create drive body structure
    DriveBody driveBody;
    driveBody.Direction = direction;
    driveBody.Duration = duration;
    driveBody.Speed = speed;

    // Copy drive body to packet data
    delete[] packet.Data;
    packet.Data = new char[sizeof(DriveBody)];
    memcpy(packet.Data, &driveBody, sizeof(DriveBody));
    packet.header.Length = HEADERSIZE + sizeof(DriveBody) + 1; // +1 for CRC
}



PktDef::DriveBody PktDef::GetDriveParams() {
    DriveBody driveBody = { 0, 0, 0 }; // Initialize to zeros

    if (packet.Data == nullptr || GetCmd() != CmdType::DRIVE) {
        return driveBody;
    }

    memcpy(&driveBody, packet.Data, sizeof(DriveBody));
    return driveBody;
}

PktDef::TelemetryBody PktDef::GetTelemetry() {
    TelemetryBody telemetry = { 0, 0, 0, 0, 0, 0 }; // Initialize to zeros

    if (packet.Data == nullptr || GetCmd() != CmdType::RESPONSE) {
        return telemetry;
    }

    memcpy(&telemetry, packet.Data, sizeof(TelemetryBody));
    return telemetry;
}

bool PktDef::ValidateCmd() const {
    // Count how many command flags are set (excluding Ack)
    int cmdCount = 0;
    if (packet.header.Command.Drive) cmdCount++;
    if (packet.header.Command.Status) cmdCount++;
    if (packet.header.Command.Sleep) cmdCount++;

    // Check that only one command flag is set at a time
    if (cmdCount > 1) {
        return false;
    }

    // Check that at least one command flag is set
    if (cmdCount == 0) {
        return false;
    }

    // If Ack is set, verify it's set with a corresponding command flag
    if (packet.header.Command.Ack) {
        // At least one command flag must be set with Ack
        return cmdCount == 1;
    }

    return true;
}