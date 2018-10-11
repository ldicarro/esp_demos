/*
  Testing the capacitive touch capabilities to turn an led on when touched.
  This feature is on the esp32 only.

  1) Connect a wire to the T0 pin (refer to the datasheet).
  2) Upload to the ESP32 module
  3) Touch the end of the wire to turn the led on, let go to turn it off.
*/

const int LED_BUILTIN = 2;  // led on board, can also use an external led
const int TPIN = T0;        // constant for the touch pin. refer to datasheet for your module
int t_val = 100;            // setting the base value of the touch sensor

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("touch test");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);

}

void loop() {
  t_val = touchRead(TPIN);  // read the pin to measure touch value
  Serial.println(t_val);    // print the value to the serial monitor
  
  // the sensor reading will drop when the pin is touched
  // set a threshold value to determine when to turn the led on and off
  if(t_val < 50)
  {
    digitalWrite(LED_BUILTIN,HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN,LOW);
  }

  delay(250);
}
