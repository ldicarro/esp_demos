/*
  This application connects to the local wi-fi network and serves a page on request.

  1) Set the wi-fi SSID and Password.
  2) Upload to the ESP32 module
  3) Use a browser on the desktop to connect to either the IP address in the Serial Monitor or http://esp32.local
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// variables for wireless SSD and password
const char *ssid = "[your network name]";
const char *password = "[your network password]";

// create the server
WebServer server(80);

// using the built in led to flash when a request has been made
const int led = LED_BUILTIN;

// default web page served
void handleRoot() {
  digitalWrite(led, 1);       // turn the led on
  char temp[400];             // create a character array to send to the requestor
  int sec = millis() / 1000;  // time calculations
  int min = sec / 60;
  int hr = min / 60;

  // print the following text back to the request
  // the %2d are filled in with the variables at the end of the text
  snprintf(temp, 400,
        "<html>\
          <head>\
            <title>ESP32 Demo</title>\
            <style>\
              body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
            </style>\
          </head>\
          <body>\
            <h1>Hello from ESP32!</h1>\
            <p>Uptime: %02d:%02d:%02d</p>\
            <a href='/on' onclick='return false;'>ON</a>\
            <a href='/off' onclick='return false;'>OFF</a>\
          </body>\
        </html>",
        hr, min % 60, sec % 60);
  server.send(200, "text/html", temp);  // http code, content type, text
  digitalWrite(led, 0);                 // turn off led
}

// print the folowing text when the url is not understood
// this is essentially a 404 - Not Found
void handleNotFound() {
  digitalWrite(led, 1); // turn the led on

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  // adding each url argument to the message 
  // ie: ?a=1&b=2 would add a = 1 and b = 2 to the message
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message); // http code, content type, text
  digitalWrite(led, 0); // turn off led
}

// turn the led on and off
// make sure something is returned to the browser 
// else the browser will timeout and error
void turnOnLED()
{
  digitalWrite(led, HIGH);
  server.send(200, "text/plain", "the led is on");
}

void turnOffLED()
{
  digitalWrite(led, LOW);
  server.send(200, "text/plain", "the led is off");
}

void setup(void) {
  pinMode(led, OUTPUT); // set led pin to output
  digitalWrite(led, 0); // turn off led


  Serial.begin(115200); // start the serial port at 115200bps
  WiFi.mode(WIFI_STA);  // turn on wifi
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // once connected - print ip address to Serial Monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // set a friendly address so the request can be words instead an IP address
  // change esp32 to any string
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  // request handlers - depending on the path given, call the function
  // ie: if http://esp32.local is sent, call handleRoot
  // if http://esp32.local/on is sent, call turnOnLED
  server.on("/", handleRoot);
  server.on("/on", turnOnLED);
  server.on("/off", turnOffLED);
  server.on("/inline", []() { server.send(200, "text/plain", "this works as well"); }); // this one shows an example of not calling a function, just returning a string
  server.onNotFound(handleNotFound);
  server.begin(); // start the server
  Serial.println("HTTP server started");
  
  // blink the led a couple of times so we now the unit is ready
  for(int i = 0; i < 10; i++)
  {
    digitalWrite(led,HIGH);
    delay(100);
    digitalWrite(led,LOW);
    delay(100);
  }
}

// hang out and wait for requests
void loop(void) {
  server.handleClient();
}