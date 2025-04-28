// Get the current hostname and port
const API_BASE_URL = window.location.origin + '/api';
let statusUpdateInterval;

// Update the status of all relays
async function updateStatus() {
    try {
        const response = await fetch(`${API_BASE_URL}/status`);
        const data = await response.json();
        
        data.relays.forEach((state, index) => {
            const indicator = document.querySelector(`#relay-${index} .status-indicator`);
            indicator.classList.toggle('on', state);
        });
    } catch (error) {
        console.error('Error fetching status:', error);
    }
}

// Control a specific relay (turn on or off)
async function controlRelay(relayId, turnOn) {
    try {
        const response = await fetch(`${API_BASE_URL}/control/${relayId}`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ state: turnOn })
        });
        const data = await response.json();
        
        if (data.success) {
            const indicator = document.querySelector(`#relay-${relayId} .status-indicator`);
            indicator.classList.toggle('on', data.state);
        }
    } catch (error) {
        console.error('Error controlling relay:', error);
    }
}

// Initialize the application
function init() {
    // Update status immediately
    updateStatus();
    
    // Set up periodic status updates
    statusUpdateInterval = setInterval(updateStatus, 2000);
}

// Start the application when the page loads
document.addEventListener('DOMContentLoaded', init); 