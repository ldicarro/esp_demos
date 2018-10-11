/*
  Control the color of a neopixel on the Sparkfun Blynk Board from a web page.

  1) Set the wi-fi SSID and Password.
  2) Upload to the Blynk Board module
  3) Use a browser on the desktop to load the accompanying web page
  4) Adjust the values on the web page to change the color of the neopixel
*/

// libraries for wi-fi
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// library for neopixel
#include <Adafruit_NeoPixel.h>

// variables for wireless SSD and password
const char *ssid = "[your network name]";
const char *password = "[your network password]";

#define WS2812_PIN 4 // Pin connected to WS2812 LED (neopixel)
Adafruit_NeoPixel rgb = Adafruit_NeoPixel(1, WS2812_PIN, NEO_GRB + NEO_KHZ800); // initialize the neopixel

ESP8266WebServer server(80); // initialize the server

// default web page served
void handleRoot() 
{
  char temp[400]; // create a character array to send to the requestor

  // print the following text back to the request
  snprintf(temp, 400,
    "<html>\
      <head>\
        <title>ESP8266 Demo</title>\
        <style>\
          body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
        </style>\
      </head>\
      <body>\
        <h1>Hello from ESP8266!</h1>\
      </body>\
    </html>");
  server.send(200, "text/html", temp); // http code, content type, text
}

// read the url and print
void handlePixel()
{
  // print the values received in the URL request to the Serial Monitor
  // ie: http://esp8266.local/pixel?r=200&g=200&b=200
  Serial.print("R = ");
  Serial.println(server.arg(0));
  Serial.print("G = ");
  Serial.println(server.arg(1));
  Serial.print("B = ");
  Serial.println(server.arg(2));
  Serial.println("--------------------------");

  // convert values from string to integers
  byte red = server.arg(0).toInt();
  byte green = server.arg(1).toInt();
  byte blue = server.arg(2).toInt();

  // create color for the neopixel and send it to the component
  uint32_t rgbColor = rgb.Color(red, green, blue);
  rgb.setPixelColor(0, rgbColor);
  rgb.show();
  
  // send a response to the browser so it does not time out and error
  server.send(200,"text/plain","done");
}

// print the folowing text when the url is not understood
// this is essentially a 404 - Not Found
void handleNotFound()
{
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
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message); // http code, content type, text
  digitalWrite(led, 0);                    // turn off led
}

void setup(void) {
  rgb.begin(); // initialize RGB LED

  Serial.begin(115200); // start the serial port at 115200bps
  WiFi.mode(WIFI_STA);  // turn on wifi
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
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
  // change esp8266 to any string
  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  // request handlers - depending on the path given, call the function
  // ie: if http://esp8266.local is sent, call handleRoot
  server.on("/", handleRoot);
  server.on("/pixel", handlePixel);
  server.on("/inline", []() { server.send(200, "text/plain", "this works as well"); }); // this one shows an example of not calling a function, just returning a string
  server.onNotFound(handleNotFound);
  server.begin(); // start the server
  Serial.println("HTTP server started");
}

// hang out and wait for requests
void loop(void) {
  server.handleClient();
}
