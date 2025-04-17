// Global variables to track current settings
let currentDuration = 10;
let currentSpeed = 90;

// Update duration value when slider changes
function updateDuration() {
  currentDuration = parseInt(document.getElementById("duration").value);
  document.getElementById("duration-value").innerText = currentDuration;
}

// Update speed value when slider changes
function updateSpeed() {
  currentSpeed = parseInt(document.getElementById("speed").value);
  document.getElementById("speed-value").innerText = currentSpeed;
}

// Connect to the robot
async function connect() {
  try {
    const ip = document.getElementById("ip").value;
    const port = document.getElementById("port").value;
    
    if (!ip) {
      updateOutput("Error: Please enter an IP address");
      return;
    }
    
    updateOutput("Connecting to robot at " + ip + ":" + port + "...");
    
    const response = await fetch(`/connect/${ip}/${port}`, { 
      method: "POST" 
    });
    
    const text = await response.text();
    updateOutput(text);
  } catch (error) {
    updateOutput("Connection error: " + error.message);
  }
}

// Send drive command to the robot
async function sendDrive(direction) {
  try {
    if (direction === "SLEEP") {
      updateOutput("Sending SLEEP command...");
      const response = await fetch("/sleep", { method: "PUT" });
      const text = await response.text();
      updateOutput(text);
      return;
    }
    
    // Create command body based on direction
    let body;
    switch (direction) {
      case "FORWARD":
        body = { dir: 1 };
        break;
      case "BACKWARD":
        body = { dir: 2 };
        break;
      case "RIGHT":
        body = { dir: 3 };
        break;
      case "LEFT":
        body = { dir: 4 };
        break;
      default:
        updateOutput("Error: Unknown direction");
        return;
    }
    
    // Add duration and speed to command
    body.duration = currentDuration;
    body.speed = currentSpeed;
    
    updateOutput(`Sending ${direction} command for ${currentDuration}s at ${currentSpeed}% speed...`);
    
    const response = await fetch("/telecommand/", {
      method: "PUT",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(body)
    });
    
    const text = await response.text();
    updateOutput(text);
  } catch (error) {
    updateOutput("Command error: " + error.message);
  }
}

// Request telemetry data from the robot
async function getTelemetry() {
  try {
    updateOutput("Requesting telemetry data...");
    
    const response = await fetch("/telementry_request/");
    
    if (!response.ok) {
      throw new Error(`HTTP error ${response.status}`);
    }
    
    const text = await response.text();
    updateOutput(text);
  } catch (error) {
    updateOutput("Telemetry error: " + error.message);
  }
}

// Update the output display
function updateOutput(message) {
  const output = document.getElementById("output");
  const timestamp = new Date().toLocaleTimeString();
  output.innerText = `[${timestamp}] ${message}\n${output.innerText}`;
}

// Initialize the page
document.addEventListener("DOMContentLoaded", function() {
  // Set default IP to localhost
  document.getElementById("ip").value = "127.0.0.1";
  
  // Initialize slider values
  updateDuration();
  updateSpeed();
  
  // Welcome message
  updateOutput("Robot Control Interface ready. Please connect to a robot.");
});
