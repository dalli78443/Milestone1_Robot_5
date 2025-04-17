# Dockerfile for Robot Control Server
FROM ubuntu:latest

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libboost-all-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files
COPY . .

# Create build directory
RUN mkdir -p build

# Build the application
WORKDIR /app/build
RUN cmake .. && make

# Expose port
EXPOSE 23500

# Run the server
CMD ["./RobotControlServer"]
