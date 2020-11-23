#include <Arduino.h>
#include "PanelClient.h"


Switch::Switch(int id, int pin, PanelClient* panel_client) :
    id_(id), pin_(pin), state_(0), panel_client_(panel_client) {
    panel_client_->addSwitch(this);
}

void Switch::poll() {
    int state = digitalRead(pin_);
    if (state_ != state) {
        state_ = state;
        panel_client_->switchUpdated(id_, state_);
    }
}

LED::LED(int id, int pin, PanelClient* panel_client) :
    id_(id), pin_(pin), state_(0) {
    panel_client->addLED(this);
}

void PanelClient::addSwitch(Switch* s) {
    switches_[s->id()] = s;
}

void PanelClient::addLED(LED* led) {
    leds_[led->id()] = led;
}

void PanelClient::switchUpdated(int id, int state) {
    // turn the corresponding bit on or off 
    if (state == 0) {
        switch_status_ = switch_status_ & ~((unsigned char)1 << id);
    } else {
        switch_status_ = switch_status_ | ((unsigned char)1 << id);
    }

    // Mark the switch updated
    switch_updated_ = switch_updated_ | ((unsigned char)1 << id);

    Serial.print("SWITCH UPDATE: sw=");
    Serial.print(id);
    Serial.print(", state=");
    Serial.print(state);
    Serial.print(": switch_status=");
    Serial.print(switch_status_, 2);
    Serial.print(", switch_updated=");
    Serial.println(switch_updated_, 2);

}

void PanelClient::processMessage(const ProtocolMessage& message) {
    Serial.print("Got message: ");
    Serial.println(message.toString());
    
    if (connected_) {

    } else {
        if (message.message_type() == MessageType::Hello) {
            connected_ = true;
        }
    }
}

bool PanelClient::getMessage(ProtocolMessage* message) {
    long now = millis();
    if (connected_) {
        if (now - last_status_update_ > 1000) {
            *message = ProtocolMessage(MessageType::StatusBegin);
            last_status_update_ = now;
            return true;
        }
    } else {
        if (now - last_helo_timestamp_ > 1000) {
            // Time to say hello...
            *message = ProtocolMessage(MessageType::Hello);
            last_helo_timestamp_ = now;
            return true;
        }
    }
    return false;
}