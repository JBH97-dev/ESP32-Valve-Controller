#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Web server on port 80
WebServer server(80);

// Valve control pins (adjust according to your setup)
const int VALVE_PINS[] = {2, 4, 5, 18}; // GPIO pins for valves 1-4
const int VALVE_COUNT = 4;

// Valve states
bool valveStates[VALVE_COUNT] = {false, false, false, false};

void setup() {
  Serial.begin(115200);
  
  // Initialize valve pins
  for (int i = 0; i < VALVE_COUNT; i++) {
    pinMode(VALVE_PINS[i], OUTPUT);
    digitalWrite(VALVE_PINS[i], LOW); // Start with valves off
  }
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
  
  // Define web server routes
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/valve/1/on", []() { controlValve(0, true); });
  server.on("/valve/1/off", []() { controlValve(0, false); });
  server.on("/valve/2/on", []() { controlValve(1, true); });
  server.on("/valve/2/off", []() { controlValve(1, false); });
  server.on("/valve/3/on", []() { controlValve(2, true); });
  server.on("/valve/3/off", []() { controlValve(2, false); });
  server.on("/valve/4/on", []() { controlValve(3, true); });
  server.on("/valve/4/off", []() { controlValve(3, false); });
  server.on("/valves/all/on", handleAllValvesOn);
  server.on("/valves/all/off", handleAllValvesOff);
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  // Serve the HTML file
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(404, "text/plain", "File not found");
    return;
  }
  
  server.streamFile(file, "text/html");
  file.close();
}

void handleStatus() {
  String json = "{";
  for (int i = 0; i < VALVE_COUNT; i++) {
    json += "\"valve" + String(i + 1) + "\":" + (valveStates[i] ? "true" : "false");
    if (i < VALVE_COUNT - 1) json += ",";
  }
  json += "}";
  
  server.send(200, "application/json", json);
}

void controlValve(int valveIndex, bool state) {
  if (valveIndex >= 0 && valveIndex < VALVE_COUNT) {
    valveStates[valveIndex] = state;
    digitalWrite(VALVE_PINS[valveIndex], state ? HIGH : LOW);
    
    Serial.print("Valve ");
    Serial.print(valveIndex + 1);
    Serial.print(" turned ");
    Serial.println(state ? "ON" : "OFF");
    
    server.send(200, "application/json", "{\"success\":true}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid valve index\"}");
  }
}

void handleAllValvesOn() {
  for (int i = 0; i < VALVE_COUNT; i++) {
    valveStates[i] = true;
    digitalWrite(VALVE_PINS[i], HIGH);
  }
  Serial.println("All valves turned ON");
  server.send(200, "application/json", "{\"success\":true}");
}

void handleAllValvesOff() {
  for (int i = 0; i < VALVE_COUNT; i++) {
    valveStates[i] = false;
    digitalWrite(VALVE_PINS[i], LOW);
  }
  Serial.println("All valves turned OFF");
  server.send(200, "application/json", "{\"success\":true}");
}
