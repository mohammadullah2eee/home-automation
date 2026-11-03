/*
  ESP32 Wi-Fi Home Automation
  ---------------------------------------------------------------
  Controls a fan and a light (LED) through a 2-channel relay module
  from any phone or laptop, using a web page served by the ESP32.

  Author   : Mohammad Ullah
  Dept.    : Electrical and Electronic Engineering (EEE), RUET, Bangladesh
  GitHub   : https://github.com/mohammadullah2eee/home-automation
  Portfolio: https://mohammadullah2eee.github.io

  How to use
    1. Upload this sketch to an ESP32 board (Arduino IDE, board "ESP32 Dev Module").
    2. Join the Wi-Fi network named in "ssid" below.
    3. Open http://192.168.4.1 in a browser and press ON / OFF.

  Wiring
    GPIO16 -> relay IN1 (fan)      GPIO17 -> relay IN2 (light)
    Relay module VCC -> 5V, GND -> GND.  The relay module is active-LOW.

  Step-by-step guide and wiring diagram: see README.md and BUILD_GUIDE.md
  in the GitHub repository above. Feel free to learn from this project.
  Please keep this header when you share or modify the code.

  Safety: test with low-voltage loads first. Mains voltage can kill.
*/

#include <WiFi.h>

const char* ssid = "ESP_HomeAutomation";
const char* password = "";   // empty = open network. Use at least 8 characters for a password.

WiFiServer server(80);
String header;

// Relay pins
const int relayFan = 16;
const int relayLED = 17;

String stateFan = "off";
String stateLED = "off";

void setup() {
  Serial.begin(115200);

  pinMode(relayFan, OUTPUT);
  pinMode(relayLED, OUTPUT);

  digitalWrite(relayFan, HIGH);
  digitalWrite(relayLED, HIGH);

  WiFi.softAP(ssid, password);

  Serial.println(WiFi.softAPIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String currentLine = "";
    header = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // FAN CONTROL
            if (header.indexOf("GET /fan/on") >= 0) {
              digitalWrite(relayFan, LOW);
              stateFan = "on";
            } 
            else if (header.indexOf("GET /fan/off") >= 0) {
              digitalWrite(relayFan, HIGH);
              stateFan = "off";
            }

            // LED CONTROL
            if (header.indexOf("GET /led/on") >= 0) {
              digitalWrite(relayLED, LOW);
              stateLED = "on";
            } 
            else if (header.indexOf("GET /led/off") >= 0) {
              digitalWrite(relayLED, HIGH);
              stateLED = "off";
            }

            // HTML PAGE
            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");

            client.println("<style>");
            client.println("body { font-family: Arial; text-align: center; background: #0f172a; color: white; margin:0; }");
            client.println(".container { display:flex; flex-direction:column; justify-content:center; align-items:center; height:100vh; }");
            client.println(".card { background:#1e293b; padding:20px; border-radius:15px; margin:10px; width:250px; }");
            client.println(".button { padding:15px 30px; font-size:20px; border:none; border-radius:10px; cursor:pointer; color:white; }");
            client.println(".on { background:#22c55e; }");
            client.println(".off { background:#ef4444; }");
            client.println("</style></head>");

            client.println("<body>");
            client.println("<div class=\"container\">");
            client.println("<h1>ESP32 Smart Control</h1>");

            // FAN UI
            client.println("<div class=\"card\"><h3>Fan</h3>");
            if (stateFan.equals("off")) {
              client.println("<a href=\"/fan/on\"><button class=\"button on\">ON</button></a>");
            } else {
              client.println("<a href=\"/fan/off\"><button class=\"button off\">OFF</button></a>");
            }
            client.println("</div>");

            // LED UI
            client.println("<div class=\"card\"><h3>LED</h3>");
            if (stateLED.equals("off")) {
              client.println("<a href=\"/led/on\"><button class=\"button on\">ON</button></a>");
            } else {
              client.println("<a href=\"/led/off\"><button class=\"button off\">OFF</button></a>");
            }
            client.println("</div>");

            client.println("</div></body></html>");

            break;
          } 
          else {
            currentLine = "";
          }
        } 
        else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}
