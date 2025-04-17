# Robot Control GUI

A web-based control interface for the robot communication project. This application implements Milestone #3 of the CSCN72050 Term Project.

## Authors
- Alli Demilade
- Adil Farid

## Features

- Web-based GUI for robot control
- RESTful API for communication with the robot
- Support for all required robot commands:
  - Drive (Forward, Backward, Left, Right)
  - Sleep
  - Telemetry Request
- Real-time status and telemetry display

## Prerequisites

- C++17 compatible compiler
- CMake 3.10 or higher
- Docker (optional, for containerized deployment)

## Building and Running

### Method 1: Using Visual Studio

1. Open the solution file `Milestone1_Robot_5.sln` in Visual Studio
2. Add the WebServer.cpp file to the project
3. Build the solution
4. Run the application

### Method 2: Using CMake

```bash
# Create a build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build .

# Run the application
./robot_control_gui
```

### Method 3: Using Docker

```bash
# Build the Docker image
docker build -t robot-control-gui .

# Run the container
docker run -p 8080:8080 robot-control-gui
```

## Usage

1. Open a web browser and navigate to `http://localhost:8080`
2. Enter the robot's IP address and port number
3. Click "Connect to Robot"
4. Use the control buttons to send commands to the robot
5. View telemetry data by clicking "Get Telemetry"

## Project Structure

- `WebServer.cpp` - Main application file with Crow web server implementation
- `MySocket.h/cpp` - Socket communication class
- `pktDef.h/cpp` - Packet definition class for robot protocol
- `public/` - Web interface files
  - `index.html` - Main HTML page
  - `styles/style.css` - CSS styling
  - `scripts/main.js` - Client-side JavaScript
- `CMakeLists.txt` - CMake build configuration
- `Dockerfile` - Docker container configuration

## Testing

To test the application with the Robot_Simulator:

1. Run the Robot_Simulator.exe with the desired port number
2. Start the Robot Control GUI application
3. Connect to the simulator using the web interface
4. Send commands and verify the responses

## License

This project is part of the CSCN72050 Term Project and is subject to academic use only.
