#include "wifi_setup.h"
#include "string.h"


int    HTTP_PORT   = 80;
String HTTP_METHOD = "POST"; // or "POST"
String HOST_NAME = "http://speedsense.pagekite.me"; // hostname of web server:
String PATH_NAME   = "/save/";
int    INST_ID     = 0;

// Replace with your network credentials
const char* ssid     = "MS7549";
const char* password = "t88-24O6";


HTTPClient client;


void setup_wifi(){
    delay(2000);
    Serial.begin(115200);
    delay(100);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }
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