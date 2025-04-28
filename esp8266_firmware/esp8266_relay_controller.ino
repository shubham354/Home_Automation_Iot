#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Your_wifi-ssid";
const char* password = "Your_wifi_password";

// Relay pins configuration
const int relayPins[] = {D1, D2, D3, D4};  // GPIO pins for relays
const int numRelays = 4;
bool relayStates[numRelays] = {false};  // Track relay states

ESP8266WebServer server(80);

// Function to send response with CORS headers
void sendResponse(int code, const char* content_type, const String& content) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(code, content_type, content);
}

void setup() {
  Serial.begin(115200);
  
  // Initialize relay pins
  for (int i = 0; i < numRelays; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);  // Relays are active LOW
  }
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup server routes
  server.on("/status", HTTP_GET, handleGetStatus);
  server.on("/control/0", HTTP_POST, []() { handleControlRelay(0); });
  server.on("/control/1", HTTP_POST, []() { handleControlRelay(1); });
  server.on("/control/2", HTTP_POST, []() { handleControlRelay(2); });
  server.on("/control/3", HTTP_POST, []() { handleControlRelay(3); });
  
  // Handle OPTIONS requests for CORS
  server.onNotFound([]() {
    if (server.method() == HTTP_OPTIONS) {
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
      server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
      server.send(204);
    } else {
      server.send(404, "text/plain", "Not Found");
    }
  });
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleGetStatus() {
  StaticJsonDocument<200> doc;
  JsonArray states = doc.createNestedArray("relays");
  
  for (int i = 0; i < numRelays; i++) {
    states.add(relayStates[i]);
  }
  
  String response;
  serializeJson(doc, response);
  
  sendResponse(200, "application/json", response);
}

void handleControlRelay(int relayId) {
  if (relayId >= 0 && relayId < numRelays) {
    // Parse the request body
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    
    if (error) {
      sendResponse(400, "application/json", "{\"error\":\"Invalid request body\"}");
      return;
    }
    
    bool newState = doc["state"];
    relayStates[relayId] = newState;
    digitalWrite(relayPins[relayId], !newState);  // Invert because relays are active LOW
    
    StaticJsonDocument<200> responseDoc;
    responseDoc["success"] = true;
    responseDoc["relay"] = relayId;
    responseDoc["state"] = newState;
    
    String response;
    serializeJson(responseDoc, response);
    
    sendResponse(200, "application/json", response);
  } else {
    sendResponse(400, "application/json", "{\"error\":\"Invalid relay ID\"}");
  }
} 
