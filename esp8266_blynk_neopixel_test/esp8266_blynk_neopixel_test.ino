/*
  Test the neopixel on the Sparkfun Blynk Board by setting the color.
  Wi-Fi is not used in this sketch.

  1) Set the red, green, and blue values in the bWrite function
  2) Upload to the Blynk Board
  3) See the color change when sketch starts
*/

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_NeoPixel.h> // library for neopixel


#define WS2812_PIN 4 // Pin connected to WS2812 LED (neopixel)
Adafruit_NeoPixel rgb = Adafruit_NeoPixel(1, WS2812_PIN, NEO_GRB + NEO_KHZ800); // initialize the neopixel

// write color to neopixel
void bWrite()
{
  byte red = 255;
  byte green = 0;
  byte blue = 100;

  uint32_t rgbColor = rgb.Color(red, green, blue);
  rgb.setPixelColor(0, rgbColor);
  rgb.show();
}

void setup()
{
  Serial.begin(9600);
  rgb.begin(); // initialize neopixel
}

void loop()
{
  bWrite();
}
