@echo off
echo Robot Control Server - Startup Script
echo ===================================
echo.

REM Check if Robot_Simulator is running
tasklist /FI "IMAGENAME eq Robot_Simulator.exe" 2>NUL | find /I /N "Robot_Simulator.exe">NUL
if "%ERRORLEVEL%"=="0" (
    echo Robot Simulator is already running.
) else (
    echo Starting Robot Simulator...
    start "" "..\Robot_Simulator\Robot__Simulator.exe" 5000
    echo Robot Simulator started on port 5000.
)

echo.
echo Starting Robot Control Server...
echo.
echo Once the server is running, open your web browser and navigate to:
echo http://localhost:23500
echo.

REM Run the server executable if it exists, otherwise show build instructions
if exist "build\RobotControlServer.exe" (
    "build\RobotControlServer.exe"
) else (
    echo Server executable not found. Please build the project first:
    echo.
    echo 1. Install required dependencies (Boost libraries)
    echo 2. Create a build directory: mkdir build
    echo 3. Navigate to build directory: cd build
    echo 4. Generate build files: cmake ..
    echo 5. Build the project: cmake --build .
    echo 6. Run this script again
    echo.
    pause
)
