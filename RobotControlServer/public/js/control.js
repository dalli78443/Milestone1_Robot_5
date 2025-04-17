/**
 * Robot Control Interface - Client-side JavaScript
 * CSCN72050 Term Project - Milestone 3
 */

// API endpoints
const API = {
    connect: '/connect',
    telecommand: '/telecommand',
    telemetry: '/telemetry'
};

// Connection state
let isConnected = false;

// Direction mapping
const DIRECTIONS = {
    FORWARD: 1,
    BACKWARD: 2,
    RIGHT: 3,
    LEFT: 4
};

// Cache DOM elements
const elements = {
    // Connection elements
    statusDot: document.querySelector('.status-dot'),
    statusText: document.querySelector('.status-text'),
    ipAddress: document.getElementById('ip-address'),
    port: document.getElementById('port'),
    connectBtn: document.getElementById('connect-btn'),
    
    // Control elements
    duration: document.getElementById('duration'),
    speed: document.getElementById('speed'),
    speedValue: document.getElementById('speed-value'),
    
    // Direction buttons
    forwardBtn: document.getElementById('forward-btn'),
    leftBtn: document.getElementById('left-btn'),
    rightBtn: document.getElementById('right-btn'),
    backwardBtn: document.getElementById('backward-btn'),
    
    // Action buttons
    sleepBtn: document.getElementById('sleep-btn'),
    telemetryBtn: document.getElementById('telemetry-btn'),
    
    // Telemetry display
    telemetryDisplay: document.getElementById('telemetry-display'),
    
    // Notification
    notification: document.getElementById('notification'),
    notificationMessage: document.getElementById('notification-message'),
    notificationClose: document.getElementById('notification-close')
};

/**
 * Initialize the application
 */
function init() {
    // Set up event listeners
    setupEventListeners();
    
    // Initialize speed display
    updateSpeedDisplay();
    
    console.log('Robot Control Interface initialized');
}

/**
 * Set up all event listeners
 */
function setupEventListeners() {
    // Speed slider
    elements.speed.addEventListener('input', updateSpeedDisplay);
    
    // Connect button
    elements.connectBtn.addEventListener('click', connectToRobot);
    
    // Direction buttons
    elements.forwardBtn.addEventListener('click', () => sendDriveCommand(DIRECTIONS.FORWARD));
    elements.backwardBtn.addEventListener('click', () => sendDriveCommand(DIRECTIONS.BACKWARD));
    elements.leftBtn.addEventListener('click', () => sendDriveCommand(DIRECTIONS.LEFT));
    elements.rightBtn.addEventListener('click', () => sendDriveCommand(DIRECTIONS.RIGHT));
    
    // Action buttons
    elements.sleepBtn.addEventListener('click', sendSleepCommand);
    elements.telemetryBtn.addEventListener('click', requestTelemetry);
    
    // Notification close button
    elements.notificationClose.addEventListener('click', () => {
        elements.notification.classList.add('hidden');
    });
    
    // Keyboard controls
    document.addEventListener('keydown', handleKeyboardInput);
}

/**
 * Update the speed value display
 */
function updateSpeedDisplay() {
    elements.speedValue.textContent = `${elements.speed.value}%`;
}

/**
 * Connect to the robot
 */
async function connectToRobot() {
    const ip = elements.ipAddress.value.trim();
    const port = parseInt(elements.port.value);
    
    // Validate inputs
    if (!ip) {
        showNotification('Please enter a valid IP address', 'error');
        return;
    }
    
    if (isNaN(port) || port < 1 || port > 65535) {
        showNotification('Please enter a valid port number (1-65535)', 'error');
        return;
    }
    
    try {
        showNotification('Connecting to robot...', 'info');
        
        const response = await fetch(API.connect, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ ip, port })
        });
        
        if (response.ok) {
            isConnected = true;
            updateConnectionStatus(true);
            showNotification('Successfully connected to robot', 'success');
        } else {
            const errorText = await response.text();
            throw new Error(errorText || 'Failed to connect to robot');
        }
    } catch (error) {
        console.error('Connection error:', error);
        updateConnectionStatus(false);
        showNotification(`Connection error: ${error.message}`, 'error');
    }
}

/**
 * Update the connection status UI
 */
function updateConnectionStatus(connected) {
    if (connected) {
        elements.statusDot.classList.remove('offline');
        elements.statusDot.classList.add('online');
        elements.statusText.textContent = 'Connected';
    } else {
        elements.statusDot.classList.remove('online');
        elements.statusDot.classList.add('offline');
        elements.statusText.textContent = 'Disconnected';
        isConnected = false;
    }
}

/**
 * Send a drive command to the robot
 */
async function sendDriveCommand(direction) {
    if (!isConnected) {
        showNotification('Not connected to robot. Please connect first.', 'warning');
        return;
    }
    
    const duration = parseInt(elements.duration.value);
    const speed = parseInt(elements.speed.value);
    
    // Validate inputs
    if (isNaN(duration) || duration < 1 || duration > 255) {
        showNotification('Duration must be between 1 and 255 seconds', 'error');
        return;
    }
    
    if (isNaN(speed) || speed < 80 || speed > 100) {
        showNotification('Speed must be between 80% and 100%', 'error');
        return;
    }
    
    // Direction names for notification
    const directionNames = {
        [DIRECTIONS.FORWARD]: 'Forward',
        [DIRECTIONS.BACKWARD]: 'Backward',
        [DIRECTIONS.RIGHT]: 'Right',
        [DIRECTIONS.LEFT]: 'Left'
    };
    
    try {
        showNotification(`Sending ${directionNames[direction]} command...`, 'info');
        
        const response = await fetch(API.telecommand, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                command: 'drive',
                direction: direction,
                duration: duration,
                speed: speed
            })
        });
        
        if (response.ok) {
            showNotification(`${directionNames[direction]} command sent successfully`, 'success');
            // Auto-request telemetry after command
            setTimeout(requestTelemetry, 500);
        } else {
            const errorText = await response.text();
            throw new Error(errorText || 'Failed to send drive command');
        }
    } catch (error) {
        console.error('Drive command error:', error);
        showNotification(`Drive command error: ${error.message}`, 'error');
    }
}

/**
 * Send a sleep command to the robot
 */
async function sendSleepCommand() {
    if (!isConnected) {
        showNotification('Not connected to robot. Please connect first.', 'warning');
        return;
    }
    
    try {
        showNotification('Sending sleep command...', 'info');
        
        const response = await fetch(API.telecommand, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ command: 'sleep' })
        });
        
        if (response.ok) {
            showNotification('Sleep command sent successfully', 'success');
            // Auto-request telemetry after command
            setTimeout(requestTelemetry, 500);
        } else {
            const errorText = await response.text();
            throw new Error(errorText || 'Failed to send sleep command');
        }
    } catch (error) {
        console.error('Sleep command error:', error);
        showNotification(`Sleep command error: ${error.message}`, 'error');
    }
}

/**
 * Request telemetry data from the robot
 */
async function requestTelemetry() {
    if (!isConnected) {
        showNotification('Not connected to robot. Please connect first.', 'warning');
        return;
    }
    
    try {
        showNotification('Requesting telemetry data...', 'info');
        
        const response = await fetch(API.telemetry);
        
        if (!response.ok) {
            const errorText = await response.text();
            throw new Error(errorText || 'Failed to get telemetry data');
        }
        
        const data = await response.json();
        
        // Check if there's an error in the response
        if (data.error) {
            throw new Error(data.error);
        }
        
        // Display telemetry data
        displayTelemetry(data);
        showNotification('Telemetry data received', 'success');
    } catch (error) {
        console.error('Telemetry error:', error);
        showNotification(`Telemetry error: ${error.message}`, 'error');
    }
}

/**
 * Display telemetry data in the UI
 */
function displayTelemetry(data) {
    // Clear previous data
    elements.telemetryDisplay.innerHTML = '';
    
    // Create telemetry items
    const telemetryItems = [
        { label: 'Last Packet Counter', value: data.lastPktCounter },
        { label: 'Current Grade', value: data.currentGrade },
        { label: 'Hit Count', value: data.hitCount },
        { label: 'Last Command', value: getCommandName(data.lastCmd) },
        { label: 'Last Command Duration', value: `${data.lastCmdValue} seconds` },
        { label: 'Last Command Speed', value: `${data.lastCmdSpeed}%` }
    ];
    
    // Add items to display
    telemetryItems.forEach(item => {
        const telemetryItem = document.createElement('div');
        telemetryItem.className = 'telemetry-item';
        
        const label = document.createElement('span');
        label.className = 'telemetry-label';
        label.textContent = item.label;
        
        const value = document.createElement('span');
        value.className = 'telemetry-value';
        value.textContent = item.value;
        
        telemetryItem.appendChild(label);
        telemetryItem.appendChild(value);
        elements.telemetryDisplay.appendChild(telemetryItem);
    });
}

/**
 * Get command name from command code
 */
function getCommandName(cmdCode) {
    const commands = {
        [DIRECTIONS.FORWARD]: 'Forward',
        [DIRECTIONS.BACKWARD]: 'Backward',
        [DIRECTIONS.RIGHT]: 'Right',
        [DIRECTIONS.LEFT]: 'Left',
        0: 'None'
    };
    
    return commands[cmdCode] || `Unknown (${cmdCode})`;
}

/**
 * Show a notification message
 */
function showNotification(message, type = 'info') {
    elements.notificationMessage.textContent = message;
    elements.notification.className = `notification ${type}`;
    elements.notification.classList.remove('hidden');
    
    // Auto-hide after 5 seconds
    setTimeout(() => {
        elements.notification.classList.add('hidden');
    }, 5000);
}

/**
 * Handle keyboard input for robot control
 */
function handleKeyboardInput(event) {
    if (!isConnected) return;
    
    switch (event.key) {
        case 'ArrowUp':
            sendDriveCommand(DIRECTIONS.FORWARD);
            break;
        case 'ArrowDown':
            sendDriveCommand(DIRECTIONS.BACKWARD);
            break;
        case 'ArrowLeft':
            sendDriveCommand(DIRECTIONS.LEFT);
            break;
        case 'ArrowRight':
            sendDriveCommand(DIRECTIONS.RIGHT);
            break;
        case 's':
        case 'S':
            sendSleepCommand();
            break;
        case 't':
        case 'T':
            requestTelemetry();
            break;
    }
}

// Initialize the application when the DOM is fully loaded
document.addEventListener('DOMContentLoaded', init);
