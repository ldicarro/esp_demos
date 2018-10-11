/*
  This application connects to the local wi-fi network and serves a page on request.

  1) Set the wi-fi SSID and Password.
  2) Upload to the ESP32 module
  3) Use a browser on the desktop to connect to either the IP address in the Serial Monitor or http://esp32.local
*/

// libraries for web server
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// libraries for the sensors
#include <OneWire.h>
#include <DallasTemperature.h>

// variables for wireless SSD and password
const char *ssid = "[your network name]";
const char *password = "[your network password]";

// create the server
WebServer server(80);

// using the built in led to flash when a request has been made
const int led = LED_BUILTIN;

// setting up variables for sensor breakout
const uint8_t dataPin = 23;
const uint8_t sensorResolution = 9;
const uint8_t sensorIndex = 0;

OneWire oneWire(dataPin);
DallasTemperature sensors(&oneWire);
DeviceAddress sensorDeviceAddress;

// default web page served
// return string with sensor values
void handleRoot()
{
  digitalWrite(led, 1); // turn on the onboard LED

  sensors.requestTemperatures(); // get the data from the sensor

  // convert the data to human readable values
  String temperatureInCelcius = String(sensors.getTempCByIndex(sensorIndex),2);
  String temperatureInFahrenheit = String(sensors.getTempFByIndex(sensorIndex),2);
  String data = String("hello from esp8266!\ncurrent temp: " + temperatureInFahrenheit + "F / " + temperatureInCelcius + "C");

  // send the data string to the requestor
  server.send(200, "text/plain", data);

  digitalWrite(led, 0); // turn off the onboard LED
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
  // sensor initialization
  sensors.begin(); // start the sensor
  sensors.getAddress(sensorDeviceAddress, 0); // this is useful if there are multiple sensors
  sensors.setResolution(sensorDeviceAddress,sensorResolution);  // setting how sensitive the returned data is

  pinMode(led, OUTPUT); // set led pin to output
  digitalWrite(led, 0); // turn off led

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
  // change esp32 to any string
  if (MDNS.begin("esp32"))
  {
    Serial.println("MDNS responder started");
  }

  // request handlers - depending on the path given, call the function
  server.on("/", handleRoot);
  server.on("/inline", []() { server.send(200, "text/plain", "this works as well"); });
  server.onNotFound(handleNotFound);
  server.begin(); // start the server
  Serial.println("HTTP server started");

  // blink the led a couple of times so we now the unit is ready
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
  }
}

// hang out and wait for requests
void loop(void)
{
  server.handleClient();
}
