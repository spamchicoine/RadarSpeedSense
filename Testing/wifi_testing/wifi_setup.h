#include <WiFi.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
#include <WebServer.h>


void send_SpeedData(String);
void setup_mode();
//void send_TestData(String);
bool connectToWiFi(String, String);
void handlePost();
String extractValue(String, String);