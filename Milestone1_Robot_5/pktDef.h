//Alli Demilade
// Adil Farid
// Robot_5
// PktDef.h
#pragma once

// Command type enumeration
enum class CmdType {
    DRIVE,
    SLEEP,
    RESPONSE
};

// Direction constants
const int FORWARD = 1;
const int BACKWARD = 2;
const int RIGHT = 3;
const int LEFT = 4;

// Header size in bytes (calculated manually)
// PktCount (2 bytes) + Command flags (1 byte) + Length (1 byte) = 4 bytes
const int HEADERSIZE = 4;

class PktDef {
public:
    // Drive command body structure
    struct DriveBody {
        unsigned char Direction;
        unsigned char Duration;
        unsigned char Speed;
    };

    // Telemetry body structure
    struct TelemetryBody {
        unsigned short LastPktCounter;
        unsigned short CurrentGrade;
        unsigned short HitCount;
        unsigned char LastCmd;
        unsigned char LastCmdValue;
        unsigned char LastCmdSpeed;
    };

private:
    // Header structure
    struct Header {
        unsigned short PktCount;
        struct {
            unsigned char Drive : 1;
            unsigned char Status : 1;
            unsigned char Sleep : 1;
            unsigned char Ack : 1;
            unsigned char Padding : 4;
        } Command;
        unsigned char Length;
    };

    // Command packet structure
    struct CmdPacket {
        Header header;
        char* Data;
        char CRC;
    };

    CmdPacket packet;
    char* RawBuffer;

public:
    // Constructors
    PktDef();
    PktDef(char* rawData);

    // Set functions
    void SetCmd(CmdType cmd);
    void SetBodyData(char* data, int size);
    void SetPktCount(int count);
    void SetAck(bool isAck);
    void SetTelemetryData(unsigned short lastPktCounter, unsigned short currentGrade,
                         unsigned short hitCount, unsigned char lastCmd,
                         unsigned char lastCmdValue, unsigned char lastCmdSpeed);
    void SetDriveParams(unsigned char direction, unsigned char duration, unsigned char speed);

    // Get functions
    CmdType GetCmd();
    bool GetAck();
    int GetLength();
    char* GetBodyData();
    int GetPktCount();

    // Structure get functions
    DriveBody GetDriveParams();
    TelemetryBody GetTelemetry();

    // Validation function
    bool ValidateCmd() const;

    // CRC functions
    bool CheckCRC(char* data, int size);
    void CalcCRC();

    // Packet generation
    char* GenPacket();

    // Destructor
    ~PktDef();
};