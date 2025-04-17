// CSCN72050 Term Project - Milestone 3
// Robot Control Server
// Team: Robot_5

#define CROW_MAIN
#include "crow_all.h"
#include "../Milestone1_Robot_5/pktDef.h"
#include "../Milestone1_Robot_5/MySocket.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <thread>
#include <chrono>

// Global variables for robot connection
std::string robotIP = "127.0.0.1";
int robotPort = 5000;
MySocket* robotSocket = nullptr;
std::mutex socketMutex;
int packetCounter = 0;

// Helper function to read files from disk
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return "File not found: " + filename;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Helper function to send a command to the robot
bool sendRobotCommand(CmdType cmdType, unsigned char direction = 0, 
                      unsigned char duration = 0, unsigned char speed = 0) {
    std::lock_guard<std::mutex> lock(socketMutex);
    
    if (!robotSocket) {
        std::cerr << "Error: Robot not connected" << std::endl;
        return false;
    }
    
    try {
        // Create packet
        PktDef packet;
        packet.SetPktCount(packetCounter++);
        packet.SetCmd(cmdType);
        
        // Set drive parameters if it's a drive command
        if (cmdType == CmdType::DRIVE) {
            // Create drive body
            char driveData[3];
            driveData[0] = direction;
            driveData[1] = duration;
            driveData[2] = speed;
            
            // Set body data
            packet.SetBodyData(driveData, 3);
        }
        
        // Calculate CRC
        packet.CalcCRC();
        
        // Generate packet
        char* buffer = packet.GenPacket();
        
        // Send packet
        robotSocket->SendData(buffer, packet.GetLength());
        
        // Wait for acknowledgment
        char recvBuffer[1024];
        int bytesReceived = robotSocket->GetData(recvBuffer);
        
        if (bytesReceived <= 0) {
            std::cerr << "Error: No acknowledgment received" << std::endl;
            return false;
        }
        
        // Parse acknowledgment
        PktDef ackPacket(recvBuffer);
        
        // Check if it's a valid acknowledgment
        if (!ackPacket.GetAck()) {
            std::cerr << "Error: Invalid acknowledgment" << std::endl;
            return false;
        }
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error sending command: " << e.what() << std::endl;
        return false;
    }
}

// Helper function to request telemetry data
crow::json::wvalue requestTelemetry() {
    std::lock_guard<std::mutex> lock(socketMutex);
    crow::json::wvalue result;
    
    if (!robotSocket) {
        result["error"] = "Robot not connected";
        return result;
    }
    
    try {
        // Create telemetry request packet
        PktDef packet;
        packet.SetPktCount(packetCounter++);
        packet.SetCmd(CmdType::RESPONSE);
        packet.CalcCRC();
        
        // Send packet
        char* buffer = packet.GenPacket();
        robotSocket->SendData(buffer, packet.GetLength());
        
        // Wait for acknowledgment
        char recvBuffer[1024];
        int bytesReceived = robotSocket->GetData(recvBuffer);
        
        if (bytesReceived <= 0) {
            result["error"] = "No acknowledgment received";
            return result;
        }
        
        // Parse acknowledgment
        PktDef ackPacket(recvBuffer);
        
        // Check if it's a valid acknowledgment
        if (!ackPacket.GetAck()) {
            result["error"] = "Invalid acknowledgment";
            return result;
        }
        
        // Wait for telemetry data
        bytesReceived = robotSocket->GetData(recvBuffer);
        
        if (bytesReceived <= 0) {
            result["error"] = "No telemetry data received";
            return result;
        }
        
        // Parse telemetry data
        PktDef telemetryPacket(recvBuffer);
        
        // Check if it's a valid telemetry packet
        if (telemetryPacket.GetCmd() != CmdType::RESPONSE) {
            result["error"] = "Invalid telemetry packet";
            return result;
        }
        
        // Extract telemetry data
        char* bodyData = telemetryPacket.GetBodyData();
        if (!bodyData) {
            result["error"] = "No telemetry data in packet";
            return result;
        }
        
        // Parse telemetry data
        unsigned short* shortData = reinterpret_cast<unsigned short*>(bodyData);
        unsigned char* charData = reinterpret_cast<unsigned char*>(bodyData + 6);
        
        // Fill result
        result["lastPktCounter"] = shortData[0];
        result["currentGrade"] = shortData[1];
        result["hitCount"] = shortData[2];
        result["lastCmd"] = static_cast<int>(charData[0]);
        result["lastCmdValue"] = static_cast<int>(charData[1]);
        result["lastCmdSpeed"] = static_cast<int>(charData[2]);
        
        return result;
    }
    catch (const std::exception& e) {
        result["error"] = std::string("Error requesting telemetry: ") + e.what();
        return result;
    }
}

int main() {
    // Create CROW app
    crow::SimpleApp app;
    
    // Define routes
    
    // Root route - serves the main HTML page
    CROW_ROUTE(app, "/")
    ([]() {
        crow::response res;
        res.set_header("Content-Type", "text/html");
        res.write(readFile("RobotControlServer/public/index.html"));
        return res;
    });
    
    // CSS route - serves CSS files
    CROW_ROUTE(app, "/css/<string>")
    ([](const std::string& filename) {
        crow::response res;
        res.set_header("Content-Type", "text/css");
        res.write(readFile("RobotControlServer/public/css/" + filename));
        return res;
    });
    
    // JavaScript route - serves JS files
    CROW_ROUTE(app, "/js/<string>")
    ([](const std::string& filename) {
        crow::response res;
        res.set_header("Content-Type", "application/javascript");
        res.write(readFile("RobotControlServer/public/js/" + filename));
        return res;
    });
    
    // Connect route - establishes connection to the robot
    CROW_ROUTE(app, "/connect")
    .methods(crow::HTTPMethod::Post)
    ([](const crow::request& req) {
        auto json = crow::json::load(req.body);
        
        if (!json || !json.has("ip") || !json.has("port")) {
            return crow::response(400, "Invalid request: missing IP or port");
        }
        
        std::lock_guard<std::mutex> lock(socketMutex);
        
        // Update connection parameters
        robotIP = json["ip"].s();
        robotPort = json["port"].i();
        
        // Clean up existing socket if any
        if (robotSocket) {
            delete robotSocket;
            robotSocket = nullptr;
        }
        
        // Create new socket
        try {
            robotSocket = new MySocket(SocketType::CLIENT, robotIP, robotPort, ConnectionType::UDP);
            return crow::response(200, "Connected to robot at " + robotIP + ":" + std::to_string(robotPort));
        }
        catch (const std::exception& e) {
            return crow::response(500, std::string("Failed to connect: ") + e.what());
        }
    });
    
    // Telecommand route - sends commands to the robot
    CROW_ROUTE(app, "/telecommand")
    .methods(crow::HTTPMethod::Put)
    ([](const crow::request& req) {
        auto json = crow::json::load(req.body);
        
        if (!json || !json.has("command")) {
            return crow::response(400, "Invalid request: missing command");
        }
        
        std::string command = json["command"].s();
        
        if (command == "drive") {
            // Validate drive parameters
            if (!json.has("direction") || !json.has("duration") || !json.has("speed")) {
                return crow::response(400, "Invalid request: missing drive parameters");
            }
            
            int direction = json["direction"].i();
            int duration = json["duration"].i();
            int speed = json["speed"].i();
            
            // Validate direction
            if (direction < 1 || direction > 4) {
                return crow::response(400, "Invalid direction: must be 1-4");
            }
            
            // Validate duration
            if (duration < 1 || duration > 255) {
                return crow::response(400, "Invalid duration: must be 1-255");
            }
            
            // Validate speed
            if (speed < 80 || speed > 100) {
                return crow::response(400, "Invalid speed: must be 80-100");
            }
            
            // Send drive command
            if (sendRobotCommand(CmdType::DRIVE, direction, duration, speed)) {
                return crow::response(200, "Drive command sent successfully");
            }
            else {
                return crow::response(500, "Failed to send drive command");
            }
        }
        else if (command == "sleep") {
            // Send sleep command
            if (sendRobotCommand(CmdType::SLEEP)) {
                return crow::response(200, "Sleep command sent successfully");
            }
            else {
                return crow::response(500, "Failed to send sleep command");
            }
        }
        else {
            return crow::response(400, "Invalid command: must be 'drive' or 'sleep'");
        }
    });
    
    // Telemetry route - requests telemetry data from the robot
    CROW_ROUTE(app, "/telemetry")
    .methods(crow::HTTPMethod::Get)
    ([]() {
        crow::json::wvalue result = requestTelemetry();
        
        // Check if there was an error
        if (result.count("error") > 0) {
            return crow::response(500, result);
        }
        
        return crow::response(200, result);
    });
    
    // Start the server
    std::cout << "Starting Robot Control Server on port 23500..." << std::endl;
    std::cout << "Use a web browser to access the control interface at http://localhost:23500" << std::endl;
    app.port(23500).multithreaded().run();
    
    // Clean up
    if (robotSocket) {
        delete robotSocket;
        robotSocket = nullptr;
    }
    
    return 0;
}
