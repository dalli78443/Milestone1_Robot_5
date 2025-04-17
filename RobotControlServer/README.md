# Robot Control Server

This is a web-based control interface for the robot, implementing Milestone 3 of the CSCN72050 Term Project.

## Overview

The Robot Control Server provides a web-based GUI for controlling the robot using the UDP protocol. It uses the CROW C++ web framework to create a RESTful API that communicates with the robot and serves a web interface for user interaction.

## Features

- Web-based control interface
- Connection management
- Robot movement controls (Forward, Backward, Left, Right)
- Sleep mode
- Telemetry data display
- Responsive design
- Keyboard controls

## Project Structure

- `server.cpp` - Main server implementation using CROW
- `public/` - Static web files
  - `index.html` - Main HTML interface
  - `css/styles.css` - CSS styling
  - `js/control.js` - Client-side JavaScript

## Prerequisites

- C++17 compatible compiler
- CMake 3.10 or higher
- Boost libraries
- CROW header-only library (included)

## Building the Project

### Using CMake

```bash
mkdir build
cd build
cmake ..
make
```

### Using Docker

```bash
docker build -t robot-control-server .
docker run -p 23500:23500 robot-control-server
```

## Running the Project

1. Start the Robot Simulator:
   ```bash
   ./Robot_Simulator.exe 5000
   ```

2. Start the Robot Control Server:
   ```bash
   ./RobotControlServer
   ```

3. Open a web browser and navigate to:
   ```
   http://localhost:23500
   ```

4. In the web interface:
   - Enter the robot's IP address (default: 127.0.0.1)
   - Enter the port number (default: 5000)
   - Click "Establish Connection"
   - Use the control buttons to send commands to the robot

## API Endpoints

- `GET /` - Serves the main HTML interface
- `POST /connect` - Establishes connection to the robot
- `PUT /telecommand` - Sends commands to the robot
- `GET /telemetry` - Requests telemetry data from the robot

## Keyboard Controls

- Arrow Up: Forward
- Arrow Down: Backward
- Arrow Left: Left
- Arrow Right: Right
- S: Sleep
- T: Request Telemetry

## Authors

Team: Robot_5
