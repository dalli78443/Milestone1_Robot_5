cmake_minimum_required(VERSION 3.10)
project(RobotControlGUI)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Find required packages
find_package(Threads REQUIRED)

# Add executable
add_executable(robot_control_gui 
    WebServer.cpp
    MySocket.cpp
    pktDef.cpp
)

# Link libraries
target_link_libraries(robot_control_gui PRIVATE Threads::Threads)

# Windows-specific settings
if(WIN32)
    target_link_libraries(robot_control_gui PRIVATE wsock32 ws2_32)
endif()

# Copy public directory to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/public DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Install targets
install(TARGETS robot_control_gui DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/public DESTINATION bin)

# Output message
message(STATUS "CMake configuration completed successfully")
