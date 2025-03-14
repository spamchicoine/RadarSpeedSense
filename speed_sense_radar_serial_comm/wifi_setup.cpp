#include "wifi_setup.h"
#include "string.h"

int    HTTP_PORT   = 80;
String HTTP_METHOD = "POST"; // or "POST"
String HOST_NAME = "http://speedsense.pagekite.me"; // hostname of web server:
String PATH_NAME   = "/save/";
int    INST_ID     = 0;

// Replace with your network credentials
String ServerSSID     = "SpeedSense";
String ServerPassword = "password";

const IPAddress localIP(192, 168, 4, 1); // Your desired static IP
const IPAddress gateway(192, 168, 1, 1);  // Network gateway
const IPAddress subnet(255, 255, 255, 0); // Subnet mask

HTTPClient client;
WebServer server(80);
extern bool isConnectedToWiFi;

String extractValue(String data, String fieldName) {
  int startIdx = data.indexOf("name=\"" + fieldName + "\"");
  if (startIdx == -1) return "";

  startIdx = data.indexOf("\r\n\r\n", startIdx);
  if (startIdx == -1) return "";

  startIdx += 4;  // Move past the line break
  int endIdx = data.indexOf("\r\n", startIdx);
  if (endIdx == -1) return "";

  return data.substring(startIdx, endIdx);
}

void handlePost() {
  String targetSSID = "";
  String targetPassword = "";

  if (server.hasArg("plain")) {
    String postBody = server.arg("plain");  // Get the POST data
    //Serial.println("Received POST data:");
    //Serial.println(postBody);
    
    targetSSID = extractValue(postBody, "ssid");
    //Serial.println("Extracted SSID: " + targetSSID);
    targetPassword = extractValue(postBody, "pwd");
    //Serial.println("Extracted Password: " + targetPassword);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"status\":\"success\"}");

    // Attempt to connect to WiFi
    isConnectedToWiFi = connectToWiFi(targetSSID, targetPassword);
    //Serial.print("IP Address: ");
    //Serial.println(WiFi.localIP());  // Print assigned IP

  } else {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
  }
}

void send_SpeedData(String Data) {
  
  // Example string format:
  //String test_data = "2024-11-06-12-31-42 25.8 25.8 25.7 25.8 25.5 25.8 25.7 25.6 25.6 25.7 25.5 25.8 25.7 25.5 25.5 25.7 25.5 25.4 25.4 25.7 25.4 25.3 25.6 25.6 25.5 25.2 25.5 25.3 25.3 25.6 25.4 25.4 25.5 25.5 25.5 25.2 25.5 25.1 25.3 25.1 25.2 25.2 25.2 25.3 25.3 25.3 25.3 25.1 25.0 25.1 25.1 25.1 24.9 25.3 25.2 25.3 25.0 25.2 25.1 25.1 25.2 24.8\n2024-11-06-12-33-42 25.1 25.3 25.2 25.1 25.0 25.0 25.1 25.0 25.0 25.2 25.3 25.3 25.2 25.3 25.3 25.3 25.2 25.0 25.0 24.9 25.0 25.3 25.2 24.9 25.3 25.2 24.9 25.3 25.0 25.2 24.9 24.9 25.1 25.1 24.9 25.2 24.9 25.1 25.1 25.1 25.2 25.0 25.0 25.0 25.0 25.1 25.2 25.0 24.9 25.0 25.2 25.1 24.9 25.0 25.0 25.2 25.1 25.2 25.1 25.1 24.9 24.9 25.1 25.1 24.9 24.9 25.2 25.0 24.9 24.8 25.1 25.0 25.0 24.8 24.9 25.0 25.0 25.0 25.0 25.0\n2024-11-06-12-35-42 29.5 29.3 29.3 29.1 29.1 29.4 29.0 29.1 29.1 28.9 29.0 28.8 28.8 28.7 28.8 28.8 28.6 28.4 28.6 28.3 28.5 28.4 28.2 28.0 28.0 28.0 27.9 28.1 28.1 27.7 27.9 27.7 27.8 27.8 27.5 27.3 27.6 27.5 27.5 27.1 27.1 27.1 27.0 26.9 27.0 26.9 27.0 26.8 26.6 26.7 26.5 26.6 26.5 26.3 26.6 26.6 26.5 26.4 26.4 26.2 26.3 26.1 26.1 25.8 25.9 25.7 25.7 25.8 25.6 25.6 25.7 25.4 25.5 25.2 25.4 25.3 25.4 25.3 25.2 25.0 24.8\n2024-11-06-12-37-42 32.3 32.4 32.1 32.0 31.8 31.7 31.5 31.4 31.4 30.8 30.9 30.5 30.7 30.5 30.4 29.9 30.1 29.6 29.6 29.6 29.2 29.1 28.9 28.9 28.6 28.3 28.4 28.0 28.0 27.8 27.9 27.6 27.2 27.1 26.9 26.8 26.6 26.7 26.4 26.4 25.9 25.8 25.9 25.4 25.4 25.2 25.1 25.1\n2024-11-06-12-39-42 29.3 29.4 29.5 29.2 29.2 29.1 29.3 29.0 29.0 29.1 29.0 28.9 29.0 28.9 28.7 28.8 28.7 28.7 28.3 28.4 28.5 28.5 28.3 28.2 28.1 28.0 28.1 28.2 27.8 28.0 27.8 27.9 27.7 27.9 27.5 27.4 27.7 27.6 27.4 27.5 27.2 27.1 27.2 27.4 27.2 27.1 26.9 26.9 26.8 26.7 26.8 26.6 26.7 26.4 26.6 26.5 26.6 26.3 26.4 26.2 26.1 26.2 26.1 25.9 26.2 26.2 26.1 25.7 25.8 25.8 25.7 25.6 25.7 25.6 25.5 25.4 25.2 25.2 25.3 25.1 25.2 25.1 24.9 24.9\n2024-11-06-12-40-42 33.8 33.5 33.7 33.3 33.3 33.1 32.7 32.6 32.6 32.3 32.2 32.1 31.7 31.7 31.5 31.4 31.3 30.9 30.8 30.5 30.5 30.1 30.1 29.8 29.7 29.5 29.6 29.4 29.0 29.1 28.6 28.4 28.3 28.2 28.2 27.8 27.7 27.4 27.3 27.3 27.2 27.0 26.5 26.7 26.4 26.0 26.1 26.0 25.8 25.4 25.3 25.2 24.8";
  //Serial.println("Attempting to send data...");
  if(WiFi.status() == WL_CONNECTED){
    // start connection
    client.begin(HOST_NAME+PATH_NAME);
    // add headers
    client.addHeader("Content-Type", "text/plain");
    client.addHeader("Connection", "close");
    client.addHeader("direction", "0");
    client.addHeader("custom-text", "CDR Testing Data");

    // Send POST request
    int httpResponseCode = client.POST(Data);

    // Check the HTTP response
    if (httpResponseCode > 0) {
      String response = client.getString();
      //Serial.println("Response Code: " + String(httpResponseCode));
      //Serial.println("Response: " + response);
    } else {
      //Serial.println("Error in sending POST: " + String(httpResponseCode));
    }
    client.end();

  } else {
    //Serial.println("Connection failed.");
  }
}

void setup_mode(){

  delay(1000);
  //Serial.begin(115200);
  //Serial.println();

  WiFi.config(localIP, gateway, subnet);
  WiFi.softAP(ServerSSID, ServerPassword);
  
  server.on("/", HTTP_POST, handlePost);
  server.begin();
  //Serial.println("Server Started");

  delay(500);

  //Serial.println("IP Address: " + WiFi.softAPIP().toString());
  //Serial.print("Local IP: ");
  //Serial.println(WiFi.localIP());
  //Serial.print("WiFi Status: ");
  //Serial.println(WiFi.status());
}

bool connectToWiFi(String ssid, String password) {
  //Serial.println("Attempting to connect to WiFi...");
  
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(3000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(ssid.c_str(), password.c_str());

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {  // Try for ~5 seconds
    delay(500);
    //Serial.print(".");
    attempts++;
  }

  //Serial.print("Connecting to: ");
  //Serial.println(ssid);
  //Serial.print("With password: ");
  //Serial.println(password);
  //Serial.print("WiFi Status Code: ");
  //Serial.println(WiFi.status());
  
  if (WiFi.status() == WL_CONNECTED) {
    //Serial.println("\nConnected to WiFi!");
    //Serial.print("IP Address: ");
    //Serial.println(WiFi.localIP());
    return true;
  } else {
    //Serial.println("\nFailed to connect to WiFi.");
    WiFi.softAP(ServerSSID, ServerPassword);
  
    server.on("/", HTTP_POST, handlePost);
    server.begin();
    //Serial.println("Server Restarted after invalid credentials sent");
    return false;
  }
}