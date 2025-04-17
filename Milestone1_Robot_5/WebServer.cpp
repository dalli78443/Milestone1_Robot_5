#define CROW_MAIN
#include "crow_all.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "pktDef.h"
#include "MySocket.h"

using namespace std;
using namespace crow;

// Global packet counter for tracking commands
static int pktCounter = 1;

// Helper function to read file contents
string readFile(const string& path) {
    ifstream file(path);
    if (file) {
        stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    return "File not found";
}

// Global UDP client socket for robot communication
MySocket* udpClient = nullptr;

int main() {
    crow::SimpleApp app;

    // Serve main HTML page
    CROW_ROUTE(app, "/")([]() {
        return readFile("public/index.html");
    });

    // Serve CSS files
    CROW_ROUTE(app, "/styles/<string>")(
        [](const string& filename) {
            response res;
            res.set_header("Content-Type", "text/css");
            res.write(readFile("public/styles/" + filename));
            return res;
        }
    );

    // Serve JavaScript files
    CROW_ROUTE(app, "/scripts/<string>")(
        [](const string& filename) {
            response res;
            res.set_header("Content-Type", "application/javascript");
            res.write(readFile("public/scripts/" + filename));
            return res;
        }
    );

    // Connect to robot endpoint
    CROW_ROUTE(app, "/connect/<string>/<int>")(
        [](string ip, int port) {
            // Cleanup existing connection if any
            if (udpClient) delete udpClient;
            
            // Create new UDP client connection
            udpClient = new MySocket(SocketType::CLIENT, ip, port, ConnectionType::UDP, 1024);
            
            response res;
            res.set_header("Content-Type", "text/plain");
            res.write("Connected to robot at " + ip + ":" + to_string(port));
            res.end();
            return res;
        }
    );

    // Send drive command to robot
    CROW_ROUTE(app, "/telecommand/")(
        [](const request& req) {
            // Check if robot is connected
            if (!udpClient) {
                response res;
                res.code = 400;
                res.write("Robot not connected.");
                res.end();
                return res;
            }

            // In a real implementation, we would parse JSON
            // For our mock, we'll use hardcoded values
            int dir = 1;  // FORWARD
            int dur = 10; // 10 seconds
            int speed = 90; // 90% speed

            // Create drive command buffer
            char drive[3] = { static_cast<char>(dir), static_cast<char>(dur), static_cast<char>(speed) };

            // Create and send drive packet
            PktDef pkt;
            pkt.SetCmd(CmdType::DRIVE);
            pkt.SetPktCount(pktCounter++);
            pkt.SetBodyData(drive, 3);
            pkt.CalcCRC();
            char* raw = pkt.GenPacket();

            udpClient->SendData(raw, pkt.GetLength());

            response res;
            res.set_header("Content-Type", "text/plain");
            res.write("Drive command sent.");
            res.end();
            return res;
        }
    );

    // Send sleep command to robot
    CROW_ROUTE(app, "/sleep")(
        [](const request& req) {
            // Check if robot is connected
            if (!udpClient) {
                response res;
                res.code = 400;
                res.write("Robot not connected.");
                res.end();
                return res;
            }

            // Create and send sleep packet
            PktDef pkt;
            pkt.SetCmd(CmdType::SLEEP);
            pkt.SetPktCount(pktCounter++);
            pkt.CalcCRC();
            char* raw = pkt.GenPacket();

            udpClient->SendData(raw, pkt.GetLength());

            response res;
            res.set_header("Content-Type", "text/plain");
            res.write("Sleep command sent.");
            res.end();
            return res;
        }
    );

    // Request and receive telemetry data from robot
    CROW_ROUTE(app, "/telementry_request/")(
        [](const request& req) {
            // Check if robot is connected
            if (!udpClient) {
                response res;
                res.code = 400;
                res.write("Robot not connected.");
                res.end();
                return res;
            }

            // Send telemetry request packet
            PktDef pkt;
            pkt.SetCmd(CmdType::RESPONSE);
            pkt.SetPktCount(pktCounter++);
            pkt.CalcCRC();
            char* raw = pkt.GenPacket();
            udpClient->SendData(raw, pkt.GetLength());

            // Receive response packets
            char recvBuf1[1024], recvBuf2[1024];
            udpClient->GetData(recvBuf1);
            udpClient->GetData(recvBuf2);

            // Parse received packets
            PktDef pkt1(recvBuf1);
            PktDef pkt2(recvBuf2);

            // Identify telemetry and ack packets
            PktDef* telemetryPkt = nullptr;
            PktDef* ackPkt = nullptr;

            if (pkt1.GetCmd() == CmdType::RESPONSE && pkt1.GetLength() > 10) {
                telemetryPkt = &pkt1;
                ackPkt = &pkt2;
            } else if (pkt2.GetCmd() == CmdType::RESPONSE && pkt2.GetLength() > 10) {
                telemetryPkt = &pkt2;
                ackPkt = &pkt1;
            }

            // Check if telemetry packet was found
            if (!telemetryPkt) {
                response res;
                res.code = 500;
                res.write("Could not find telemetry packet.");
                res.end();
                return res;
            }

            // Extract telemetry data
            PktDef::TelemetryBody t = telemetryPkt->GetTelemetry();

            // Format telemetry response
            ostringstream os;
            os << "Telemetry Received\n"
               << "PktCount: " << telemetryPkt->GetPktCount() << "\n"
               << "LastCmd: " << (int)t.LastCmd << "\n"
               << "Duration: " << (int)t.LastCmdValue << " sec\n"
               << "Speed: " << (int)t.LastCmdSpeed << "%\n"
               << "Hits: " << t.HitCount << "\n"
               << "Grade: " << t.CurrentGrade;

            response res;
            res.set_header("Content-Type", "text/plain");
            res.write(os.str());
            res.end();
            return res;
        }
    );

    // Start web server on port 8080 with multithreading
    app.port(8080).multithreaded().run();
    
    return 0;
}
