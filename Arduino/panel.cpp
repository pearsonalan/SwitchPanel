#include <Arduino.h>
#include <Wstring.h>

using string = String;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  while (!Serial) {}
}

// the loop function runs over and over again forever
void loop() {
  string s("hello");
  Serial.println(s);
  delay(1000);               // wait for a second
}
