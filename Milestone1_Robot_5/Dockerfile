# Use a base image with C++ build tools
FROM ubuntu:20.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy source files
COPY . .

# Create build directory
RUN mkdir -p build

# Build the application
WORKDIR /app/build
RUN cmake .. && make

# Expose the port
EXPOSE 8080

# Set the entry point
CMD ["./robot_control_gui"]
