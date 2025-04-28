from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
import requests
import os
from dotenv import load_dotenv
from typing import List, Dict
import json
import logging
from pydantic import BaseModel
import socket
from pathlib import Path

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

load_dotenv()

app = FastAPI()

# Get local IP address
def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # doesn't even have to be reachable
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except Exception:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP

# CORS configuration
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # In production, replace with specific origins
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Get the absolute path to the static directory
BASE_DIR = Path(__file__).resolve().parent
STATIC_DIR = BASE_DIR / "static"

# Mount static files
app.mount("/static", StaticFiles(directory=str(STATIC_DIR)), name="static")

# ESP8266 configuration
ESP8266_IP = os.getenv("ESP8266_IP", "http://192.168.82.90")  # Replace with your ESP8266's IP
STATUS_ENDPOINT = f"{ESP8266_IP}/status"
CONTROL_ENDPOINT = f"{ESP8266_IP}/control"

# Cache for last known status
last_status = {"relays": [False, False, False, False]}

class RelayControl(BaseModel):
    state: bool

@app.get("/")
async def read_root():
    return FileResponse(str(STATIC_DIR / "index.html"))

@app.get("/api/status")
async def get_status():
    try:
        logger.info(f"Fetching status from ESP8266 at {STATUS_ENDPOINT}")
        response = requests.get(STATUS_ENDPOINT, timeout=5)
        response.raise_for_status()
        status = response.json()
        last_status.update(status)
        return status
    except requests.RequestException as e:
        logger.error(f"Error fetching status from ESP8266: {str(e)}")
        return last_status

@app.post("/api/control/{relay_id}")
async def control_relay(relay_id: int, control: RelayControl):
    if relay_id < 0 or relay_id > 3:
        raise HTTPException(status_code=400, detail="Invalid relay ID")
    
    try:
        logger.info(f"Setting relay {relay_id} to {'ON' if control.state else 'OFF'} at {CONTROL_ENDPOINT}/{relay_id}")
        response = requests.post(
            f"{CONTROL_ENDPOINT}/{relay_id}",
            json={"state": control.state},
            timeout=5
        )
        response.raise_for_status()
        result = response.json()
        
        # Update cache
        if result.get("success"):
            last_status["relays"][relay_id] = result["state"]
        
        return result
    except requests.RequestException as e:
        logger.error(f"Error controlling relay {relay_id}: {str(e)}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to communicate with ESP8266: {str(e)}"
        )

if __name__ == "__main__":
    import uvicorn
    local_ip = get_local_ip()
    logger.info(f"Starting server on {local_ip}:8000")
    logger.info(f"Serving static files from {STATIC_DIR}")
    uvicorn.run(app, host="0.0.0.0", port=8000) 