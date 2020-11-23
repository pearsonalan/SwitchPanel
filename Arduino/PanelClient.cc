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