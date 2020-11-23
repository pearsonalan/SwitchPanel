#include <Arduino.h>
#include <WString.h>

#include "Protocol.h"
#include "PanelClient.h"

using string = String;

Protocol protocol;
PanelClient panel_client;

Switch switch0(0, 7, &panel_client);
Switch switch1(1, 8, &panel_client);

LED led0(0, 13, &panel_client);
LED led1(1, 14, &panel_client);

// Milliseconds between polling the switches
constexpr long kSwitchPollInterval = 50;
long last_switch_poll_time = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
}

void loop() {
  long now = millis();

  // Process any incoming bytes
  while (Serial.available() > 0) {
    // read the incoming byte:
    char incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
    protocol.addBytes(&incomingByte, 1);
  }

  // process any messages
  while (protocol.hasMessage()) {
    ProtocolMessage message = protocol.getMessage();
    panel_client.processMessage(message);
  }

  // Poll switches if it is time to...
  if (now - last_switch_poll_time > kSwitchPollInterval) {
    switch0.poll();
    switch1.poll();
    last_switch_poll_time = now;
  }

  ProtocolMessage message;
  while (panel_client.getMessage(&message)) {
      string s = message.toString();
      Serial.println(s);
  } 
}
