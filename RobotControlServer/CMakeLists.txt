cmake_minimum_required(VERSION 3.10)
project(RobotControlServer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(Threads REQUIRED)

# Include directories
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../Milestone1_Robot_5)

# Add executable
add_executable(RobotControlServer server.cpp)

# Link libraries
target_link_libraries(RobotControlServer
    ${Boost_LIBRARIES}
    Threads::Threads
)

# Copy static files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/public DESTINATION ${CMAKE_BINARY_DIR})

# Install target
install(TARGETS RobotControlServer DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/public DESTINATION share/RobotControlServer)
