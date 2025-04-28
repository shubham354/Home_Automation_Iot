# Home Automation System

A complete home automation system using ESP8266, Python FastAPI backend, and a web frontend to control LED lights via relays.

## Components

1. ESP8266 Firmware
2. Python Backend (FastAPI)
3. Web Frontend

## Setup Instructions

### ESP8266 Setup

1. Install the required Arduino libraries:
   - ESP8266WiFi
   - ESP8266WebServer
   - ArduinoJson

2. Update the WiFi credentials in `esp8266_firmware/esp8266_relay_controller.ino`:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```

3. Upload the firmware to your ESP8266 using the Arduino IDE

### Python Backend Setup

1. Create a virtual environment:
   ```bash
   python -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   ```

2. Install dependencies:
   ```bash
   cd backend
   pip install -r requirements.txt
   ```

3. Create a `.env` file in the backend directory:
   ```
   ESP8266_IP=http://YOUR_ESP8266_IP
   ```

4. Run the backend server:
   ```bash
   python main.py
   ```

### Frontend Setup

1. Open `frontend/index.html` in a web browser
2. Make sure the backend server is running
3. The frontend will automatically connect to the backend and start polling for status updates

## Usage

1. Power on the ESP8266 and ensure it's connected to your WiFi network
2. Start the Python backend server
3. Open the frontend web interface
4. Use the toggle buttons to control the LED lights

## Project Structure

```
.
├── esp8266_firmware/
│   └── esp8266_relay_controller.ino
├── backend/
│   ├── main.py
│   └── requirements.txt
├── frontend/
│   ├── index.html
│   ├── styles.css
│   └── script.js
└── README.md
```

## Notes

- The ESP8266 uses GPIO pins D1-D4 for the relays
- Relays are active LOW (HIGH signal turns them off)
- The frontend polls the backend every 2 seconds for status updates
- CORS is enabled to allow frontend-backend communication 