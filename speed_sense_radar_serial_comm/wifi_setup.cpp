#include "wifi_setup.h"
#include "string.h"


int    HTTP_PORT   = 80;
String HTTP_METHOD = "POST"; // or "POST"
String HOST_NAME = "http://speedsensesdp.pagekite.me"; // hostname of web server:
String PATH_NAME   = "/save/";
int    INST_ID     = 0;

// Replace with your network credentials
const char* ssid     = "MS7549";
const char* password = "t88-24O6";
//const char *ssid = "Verizon-SM-N970U-4a7d";
//const char *password = "uqfw684*";

HTTPClient client;

void setup_wifi(){
    delay(2000);
    Serial.begin(115200);
    delay(1000);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("Connecting...")
        delay(1000);
    }
    Serial.println("Connected")
}

void send_SpeedData(String Data) {
  // Example string format:
  if(WiFi.status() == WL_CONNECTED){
    // start connection
    client.begin(HOST_NAME+PATH_NAME);
    // add headers
    client.addHeader("Content-Type", "text/plain");
    client.addHeader("Connection", "close");
    client.addHeader("direction", "0");
    client.addHeader("custom-text", "TEST LIVE");

    // Send POST request
    int httpResponseCode = client.POST(Data);

    // Check the HTTP response
    if (httpResponseCode > 0) {
      String response = client.getString();
    }
    client.end();
  }
}

void send_TestData(String Data) {
  // Example string format:
  if(WiFi.status() == WL_CONNECTED){
    // start connection
    client.begin(HOST_NAME+PATH_NAME);
    // add headers
    client.addHeader("Content-Type", "text/plain");
    client.addHeader("Connection", "close");
    client.addHeader("direction", "0");
    client.addHeader("custom-text", "MDR Demo Button");

    // Send POST request
    int httpResponseCode = client.POST(Data);

    // Check the HTTP response
    if (httpResponseCode > 0) {
      String response = client.getString();
    }
    client.end();
  }
}