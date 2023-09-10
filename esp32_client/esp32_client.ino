/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>
#include <ArduinoJson.h>        // Include the JSON formatting library

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include <Thread.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;
// Declare the HTTP client:
HTTPClient http;

// Server names and URL's:
// String SERVER_BASE_URL     = "https://plum-cockroach-gown.cyclic.app";    // Deployment server//
String SERVER_BASE_URL   = "http://192.168.1.101:5000";                      // Local testing server//


// Define the pin numbers for sensors and actuators: 
const int RELAY_PIN = 12;

// Threads for receiving the commands: 
Thread* command_thread = new Thread();

void setup() {

    USE_SERIAL.begin(115200);
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    wifiMulti.addAP("SSID", "Password");

    pinMode(RELAY_PIN, OUTPUT);

    command_thread->onRun(commandCallback);
    command_thread->setInterval(1000);  // milliseconds
}

void commandCallback() {

    if (wifiMulti.run() == WL_CONNECTED) { 

      http.begin(SERVER_BASE_URL + "/api/commands");
      Serial.println(" Command callback");

      int http_code = http.GET();

      if (http_code > 0){
        Serial.println("Successfully received command");
        // file found at server
        if(http_code == HTTP_CODE_OK) {
            String payload = http.getString();
            USE_SERIAL.println(payload);

            const int capacity = JSON_OBJECT_SIZE(3);
            StaticJsonDocument<3*capacity> commands;
      
              
             // Deserialize the JSON document
            DeserializationError error = deserializeJson(commands, payload);
      
            // Test if parsing succeeds.
            if (error) {
              Serial.print("deserializeJson() failed: ");
              Serial.println(error.c_str());
              //return;
            }
      
            if (commands["pump_command"] == "OFF" && digitalRead(RELAY_PIN) == HIGH)
            {
              digitalWrite(RELAY_PIN, LOW); 
            }
      
            if (commands["pump_command"] == "ON" && digitalRead(RELAY_PIN) == LOW)
            {
              digitalWrite(RELAY_PIN, HIGH); 
            } 
        }
      }
      else {
        Serial.println("Failed to receive command");
        Serial.println(http_code);
      }

      http.end();  // Close connection
   }

}

void loop() {
  
    // Run the command thread to poll for the latest command
    if (command_thread->shouldRun() && wifiMulti.run() == WL_CONNECTED)
    {
        command_thread->run(); 
    }
}
